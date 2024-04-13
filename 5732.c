  explicit MatrixSolveOp(OpKernelConstruction* context) : Base(context) {
    OP_REQUIRES_OK(context, context->GetAttr("adjoint", &adjoint_));
  }