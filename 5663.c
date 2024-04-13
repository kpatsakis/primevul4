  void Compute(OpKernelContext* c) override {
    core::RefCountPtr<Var> v;
    OP_REQUIRES_OK(c, LookupResource(c, HandleFromInput(c, 0), &v));
    OP_REQUIRES_OK(c, EnsureSparseVariableAccess<Device, T>(c, v.get()));
    const bool is_non_pod_dtype = c->input_dtype(0) == DT_RESOURCE ||
                                  c->input_dtype(0) == DT_STRING ||
                                  c->input_dtype(0) == DT_VARIANT;
    if (is_non_pod_dtype || use_exclusive_lock_) {
      mutex_lock ml(*v->mu());
      DoCompute(c);
    } else {
      // For POD dtypes, we can safely run the update without the mutex.
      tf_shared_lock ml(*v->mu());
      DoCompute(c);
    }
  }