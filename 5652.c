  void Compute(OpKernelContext* context) override {
    const Tensor& value = context->input(1);
    core::RefCountPtr<Var> variable;
    OP_REQUIRES_OK(context, LookupOrCreateResource<Var>(
                                context, HandleFromInput(context, 0), &variable,
                                [](Var** ptr) {
                                  // Created on host.
                                  *ptr = new Var(DT_VARIANT);
                                  return Status::OK();
                                }));

    // For purposes of forwarding DT_VARIANT, we want the least
    // restrictive attr; we already know the input is on host.
    AllocatorAttributes attr;

    // Copying is unnecessary if we are the last user of the value
    // tensor, we can just adopt the input tensor's buffer instead.
    // Note that Variant objects themselves always reside on host.
    //
    // We nevertheless want to signal to the runtime that the tensor
    // should reside in memory of the associated device, as Variant
    // tensors may be marked as sitting on either CPU or GPU.  This
    // helps to elide one or more copies.
    std::unique_ptr<Tensor> input_alias = context->forward_input(
        1, OpKernelContext::Params::kNoReservation /*output_index*/, DT_VARIANT,
        value.shape(),
        DEVICE_MEMORY /* HOST_MEMORY is only reserved for special cases */,
        attr);

    mutex_lock ml(*variable->mu());
    OP_REQUIRES(context, variable->tensor()->dtype() == DT_VARIANT,
                errors::InvalidArgument(
                    "Trying to assign variable with wrong dtype. Expected ",
                    DataTypeString(variable->tensor()->dtype()), " got ",
                    DataTypeString(DT_VARIANT)));
    variable->is_initialized = true;
    *variable->tensor() = Tensor(DT_VARIANT, value.shape());

    if (input_alias) {
      *variable->tensor() = *input_alias;
      return;
    }

    // Need to copy, but maybe we can re-use variable's buffer?
    if (!variable->tensor()->RefCountIsOne() ||
        !variable->tensor()->shape().IsSameSize(value.shape())) {
      // Allocation of DT_VARIANT is always on host.
      attr.set_on_host(true);
      OP_REQUIRES_OK(context, context->allocate_temp(DT_VARIANT, value.shape(),
                                                     variable->tensor(), attr));
    }

    const auto elements_in = value.flat<Variant>();
    auto elements_out = variable->tensor()->flat<Variant>();
    for (int64_t i = 0; i < elements_in.size(); ++i) {
      elements_out(i) = elements_in(i);
    }
  }