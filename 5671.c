void VarHandleOp::Compute(OpKernelContext* ctx) {
  if (is_anonymous_) {
    AllocatorAttributes attr;
    attr.set_on_host(true);
    Tensor handle;
    OP_REQUIRES_OK(
        ctx, ctx->allocate_temp(DT_RESOURCE, TensorShape({}), &handle, attr));
    handle.scalar<ResourceHandle>()() = MakeResourceHandle<Var>(
        ctx, container_, name_,
        std::vector<DtypeAndPartialTensorShape>{dtype_and_shape_},
        ctx->stack_trace());
    ctx->set_output(0, handle);
  } else {
    ctx->set_output(0, resource_);
  }
}