  void Compute(OpKernelContext* context) override {
    OP_REQUIRES(context, dtype_ == context->input(1).dtype(),
                errors::InvalidArgument(
                    "Variable and value dtypes don't match; respectively, ",
                    DataTypeString(dtype_), " and ",
                    DataTypeString(context->input(1).dtype())));
    core::RefCountPtr<Var> variable;
    const Tensor& value = context->input(1);
    // Note: every resource-variable-manipulating op assumes copy-on-write
    // semantics, and creates a copy of the variable's Tensor if its refcount is
    // bigger than 1 when we try to modify it. This means we never need to copy
    // the original tensor for AssignVariableOp; even if there are other live
    // users of it we know none can modify it so this is always safe (even in
    // esoteric cases where the same tensor is used to initialize multiple
    // variables or the tensor is a constant this is safe, as future writes will
    // trigger copies).
    OP_REQUIRES_OK(context, LookupOrCreateResource<Var>(
                                context, HandleFromInput(context, 0), &variable,
                                [this, &value](Var** ptr) {
                                  *ptr = new Var(dtype_);
                                  *(*ptr)->tensor() = value;
                                  (*ptr)->is_initialized = true;
                                  return Status::OK();
                                }));
    mutex_lock ml(*variable->mu());
    // (variable->tensor()->dtype() == DT_INVALID && !variable->is_initialized)
    // check below is to allow an XLA specific situation wherein update can
    // happen first by the AssignVariableOp,
    // in which case the variable is still uninitialized.
    // When using TF-XLA, this scenario is possible when the execution uses the
    // 'fallback' path (which essentially invokes Tensorflow ops via
    // partitioned_call).
    OP_REQUIRES(context,
                (variable->tensor()->dtype() == DT_INVALID &&
                 !variable->is_initialized) ||
                    variable->tensor()->dtype() == dtype_,
                errors::InvalidArgument(
                    "Trying to assign variable with wrong dtype. Expected ",
                    DataTypeString(variable->tensor()->dtype()), " got ",
                    DataTypeString(dtype_)));
    if (variable->copy_on_read_mode.load()) {
      AllocatorAttributes attr;
      attr.set_gpu_compatible(true);
      attr.set_nic_compatible(true);
      OP_REQUIRES_OK(context,
                     context->allocate_temp(value.dtype(), value.shape(),
                                            variable->tensor(), attr));
      functor::DenseUpdate<Device, T, ASSIGN> copy_functor;
      copy_functor(context->eigen_device<Device>(),
                   variable->tensor()->flat<T>(), value.flat<T>());
    } else {
      *variable->tensor() = value;
    }
    variable->is_initialized = true;
  }