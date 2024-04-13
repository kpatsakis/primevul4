ReadVariableOp::ReadVariableOp(OpKernelConstruction* c) : OpKernel(c) {
  OP_REQUIRES_OK(c, c->GetAttr("dtype", &dtype_));
}