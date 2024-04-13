  explicit BoostedTreesQuantileStreamResourceDeserializeOp(
      OpKernelConstruction* const context)
      : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr(kNumStreamsName, &num_features_));
  }