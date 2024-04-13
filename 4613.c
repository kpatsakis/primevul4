  explicit BoostedTreesCreateQuantileStreamResourceOp(
      OpKernelConstruction* const context)
      : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr(kMaxElementsName, &max_elements_));
  }