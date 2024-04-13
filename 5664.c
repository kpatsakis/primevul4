DestroyResourceOp::DestroyResourceOp(OpKernelConstruction* ctx)
    : OpKernel(ctx) {
  OP_REQUIRES_OK(ctx,
                 ctx->GetAttr("ignore_lookup_error", &ignore_lookup_error_));
}