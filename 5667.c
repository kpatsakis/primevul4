  explicit AssignVariableOp(OpKernelConstruction* c) : OpKernel(c) {
    OP_REQUIRES_OK(c, c->GetAttr("dtype", &dtype_));
    OP_REQUIRES(c, dtype_ == DT_VARIANT,
                errors::Internal("Variant kernel called with dtype: ",
                                 DataTypeString(dtype_)));
  }