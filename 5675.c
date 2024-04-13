ReadVariablesOp::ReadVariablesOp(OpKernelConstruction* c) : OpKernel(c) {
  int n;
  OP_REQUIRES_OK(c, c->GetAttr("N", &n));
  OP_REQUIRES_OK(c, c->GetAttr("dtypes", &dtypes_));
  OP_REQUIRES(c, n == dtypes_.size(),
              errors::InvalidArgument(
                  "Mismatched number of arguments to ReadVariablesOp (", n,
                  " vs. ", dtypes_.size(), ")"));
}