  explicit MatrixSolveOpGpu(OpKernelConstruction* context)
      : AsyncOpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("adjoint", &adjoint_));
  }