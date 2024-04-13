void DestroyResourceOp::Compute(OpKernelContext* ctx) {
  const ResourceHandle& p = HandleFromInput(ctx, 0);
  Status status = DeleteResource(ctx, p);
  if (ignore_lookup_error_ && errors::IsNotFound(status)) {
    return;
  }
  OP_REQUIRES_OK(ctx, status);
}