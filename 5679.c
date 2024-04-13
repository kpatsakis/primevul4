  explicit ResourceScatterUpdateOp(OpKernelConstruction* c) : OpKernel(c) {
    // We use the same kernel for many operations.
    // Each operation has a different set of attributes defined in its nodes.
    Status s = c->GetAttr("use_locking", &use_exclusive_lock_);
    if (!s.ok()) {
      use_exclusive_lock_ = false;
    }
  }