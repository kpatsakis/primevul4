  void Compute(OpKernelContext* context) override {
    core::RefCountPtr<Var> variable;
    OP_REQUIRES_OK(context, LookupResource(context, HandleFromInput(context, 0),
                                           &variable));

    const Tensor& value = context->input(1);
    // TODO(apassos): We could possibly avoid the copy done by
    // PrepareToUpdateVariable() for commutative operations like Op ==
    // ADD if value's refcount was 1.
    mutex_lock ml(*variable->mu());
    Tensor* var_tensor = variable->tensor();
    OP_REQUIRES(context, var_tensor->shape().IsSameSize(value.shape()),
                errors::InvalidArgument("Cannot update variable with shape ",
                                        var_tensor->shape().DebugString(),
                                        " using a Tensor with shape ",
                                        value.shape().DebugString(),
                                        ", shapes must be equal."));
    OP_REQUIRES_OK(
        context, PrepareToUpdateVariable<Device, T>(
                     context, var_tensor, variable->copy_on_read_mode.load()));
    functor::DenseUpdate<Device, T, Op> update_functor;
    update_functor(context->eigen_device<Device>(), var_tensor->flat<T>(),
                   value.flat<T>());
  }