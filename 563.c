  void Compute(OpKernelContext* context) override {
    const Tensor& rhs = context->input(1);

    // We always return the input ref.
    context->forward_ref_input_to_ref_output(0, 0);

    // We can't always know how this value will be used downstream, so make
    // conservative assumptions in specifying constraints on the memory
    // allocation attributes, unless the Grappler graph analysis determined that
    // it was safe not to.
    AllocatorAttributes attr;
    if (!relax_constraints_) {
      attr.set_gpu_compatible(true);
      attr.set_nic_compatible(true);
    }

    {
      mutex_lock l(*context->input_ref_mutex(0));
      const Tensor& old_lhs = context->mutable_input(0, /* lock_held */ true);
      const bool same_shape = old_lhs.shape().IsSameSize(rhs.shape());
      if (validate_shape_) {
        OP_REQUIRES(context, same_shape,
                    errors::InvalidArgument(
                        "Assign requires shapes of both tensors to match. "
                        "lhs shape= ",
                        old_lhs.shape().DebugString(),
                        " rhs shape= ", rhs.shape().DebugString()));
      }

      // In the code below we try to minimize the amount of memory allocation
      // and copying by trying the following two shortcuts:
      // 1. If the lhs is initialized and has the same number of elements as
      //    the rhs we can avoid a memory allocation.
      // 2. If we can reuse the rhs buffer we avoid both a memory allocation
      //    and copying.

      // 1. Try to copy into an existing buffer.
      if (old_lhs.IsInitialized() &&
          old_lhs.shape().num_elements() == rhs.shape().num_elements()) {
        // The existing lhs tensor has already been initialized and the right
        // hand side can fit in the underlying buffer.
        Tensor reshaped_old_lhs;
        if (same_shape) {
          reshaped_old_lhs = old_lhs;
        } else {
          CHECK(reshaped_old_lhs.CopyFrom(old_lhs, rhs.shape()));
          context->replace_ref_input(0, reshaped_old_lhs,
                                     /* lock_held */ true);
        }
        if (use_exclusive_lock_) {
          Copy(context, &reshaped_old_lhs, rhs);
          return;
        }
      } else {
        // 2. Try to reuse the rhs.
        std::unique_ptr<Tensor> input_alias = context->forward_input(
            1, OpKernelContext::Params::kNoReservation /*output_index*/,
            rhs.dtype(), rhs.shape(), DEVICE_MEMORY, attr);
        if (input_alias != nullptr) {
          // Update the ref to point to the new buffer.
          context->replace_ref_input(0, *input_alias, /* lock_held */ true);
          return;
        }

        // Otherwise, create a new tensor whose shape matches the
        // right hand side, hand off to lhs and copy the rhs into it.
        Tensor copy_tensor;
        OP_REQUIRES_OK(context,
                       context->allocate_temp(old_lhs.dtype(), rhs.shape(),
                                              &copy_tensor, attr));
        // We track memory of variables in variable ops instead of in this
        // assign op.
        context->clear_recorded_memory();
        context->replace_ref_input(0, copy_tensor, /* lock_held */ true);
        if (use_exclusive_lock_) {
          Copy(context, &copy_tensor, rhs);
          return;
        }
      }
    }

    // The tensor has already been initialized and the right hand side
    // matches the left hand side's shape. We have been told to do the
    // copy outside the lock.
    Tensor old_unlocked_lhs = context->mutable_input(0, /* lock_held */ false);
    Copy(context, &old_unlocked_lhs, rhs);
  }