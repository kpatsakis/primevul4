  explicit BoostedTreesQuantileStreamResourceFlushOp(
      OpKernelConstruction* const context)
      : OpKernel(context) {
    OP_REQUIRES_OK(context,
                   context->GetAttr(kGenerateQuantiles, &generate_quantiles_));
  }