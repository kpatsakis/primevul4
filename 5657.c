  explicit AssignVariableOp(OpKernelConstruction* c) : OpKernel(c) {
    OP_REQUIRES_OK(c, c->GetAttr("dtype", &dtype_));
    if (!c->GetAttr("_grappler_relax_allocator_constraints",
                    &relax_constraints_)
             .ok()) {
      relax_constraints_ = false;
    }
  }