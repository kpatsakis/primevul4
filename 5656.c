  void Compute(OpKernelContext* context) override {
    Tensor* output = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output(0, TensorShape({}), &output));
    auto output_tensor = output->tensor<bool, 0>();
    core::RefCountPtr<Var> variable;
    Status s = LookupResource(context, HandleFromInput(context, 0), &variable);
    if (!s.ok()) {
      output_tensor() = false;
      return;
    }
    mutex_lock ml(*variable->mu());
    output_tensor() = variable->is_initialized;
  }