bool KernelCacheEnabled(const OpDef& op_def) {
  if (data::DatasetOpKernel::IsDatasetOp(op_def)) {
    return false;
  }
  // TODO(b/162540360): Revisit a way to mark kernels as uncachable once we have
  // 5+ kernels to exclude.
  return true;
}