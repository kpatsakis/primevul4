void ReadVariablesOp::Compute(OpKernelContext* ctx) {
  std::vector<core::RefCountPtr<Var>> variables(dtypes_.size());
  std::vector<const ResourceHandle*> handles(dtypes_.size());
  for (size_t i = 0; i < dtypes_.size(); ++i) {
    handles[i] = &HandleFromInput(ctx, i);
  }

  OP_REQUIRES_OK(ctx, LookupResources(ctx, handles, &variables));

  std::vector<string> uninitialized_vars;
  for (int64_t i = 0; i < variables.size(); i++) {
    if (variables[i] == nullptr) {
      uninitialized_vars.push_back(handles[i]->name());
    }
  }

  OP_REQUIRES(ctx, uninitialized_vars.empty(),
              errors::FailedPrecondition(
                  "In ReadVariablesOp the following variables were "
                  "found uninitialized: ",
                  absl::StrJoin(uninitialized_vars, ", ")));

  for (size_t i = 0; i < dtypes_.size(); ++i) {
    // We're acquiring a reference to the underlying buffer while
    // holding a shared lock to guarantee ordering of reads and
    // writes.
    tf_shared_lock ml(*variables[i]->mu());
    OP_REQUIRES(ctx, dtypes_[i] == variables[i]->tensor()->dtype(),
                errors::InvalidArgument(
                    "Trying to read variable ", handles[i]->name(),
                    " from Container: ", handles[i]->container(),
                    " with wrong dtype. Expected ", DataTypeString(dtypes_[i]),
                    " got ", DataTypeString(variables[i]->tensor()->dtype())));
    if (variables[i]->copy_on_read_mode.load()) {
      OP_REQUIRES_OK(ctx, CopyVariable(i, ctx, variables[i]->tensor()));
    } else {
      const Tensor& t = *variables[i]->tensor();
      ctx->set_output(i, t);
    }
  }
}