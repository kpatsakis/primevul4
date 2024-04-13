  explicit BoostedTreesQuantileStreamResourceGetBucketBoundariesOp(
      OpKernelConstruction* const context)
      : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr(kNumFeaturesName, &num_features_));
  }