StatusOr<Fprint128> GetKernelCacheKey(
    const EagerOperation& op, const Fprint128& op_cache_key,
    const std::vector<Device*>& input_dev_ptrs,
    const std::unordered_map<int, DtypeAndPartialTensorShape>&
        input_resource_variable_dtypes_and_shapes) {
  EagerContext& ctx = op.EagerContext();

  Fprint128 cache_key = op_cache_key;
  /// Include soft placement policy in cache key since the placement strategy
  // can change and thus affect which kernel is picked.
  cache_key = FingerprintCat128(cache_key, ctx.AllowSoftPlacement());

  // Include run_eager_op_as_function policy in cache key since the execution
  // strategy can change and affect which kernel is picked.
  VLOG(3) << "ctx.RunEagerOpAsFunction(): " << ctx.RunEagerOpAsFunction();
  cache_key = FingerprintCat128(cache_key, ctx.RunEagerOpAsFunction());

  // When running in eager_op_as_function mode Send/Recv ops need to be
  // placed on the same rendezvous to match the behaviour of eager mode.
  bool reuse_rendezvous_for_functions =
      (ctx.RunEagerOpAsFunction() && !op.is_function()) ||
      ctx.GetReuseRendezvousForFunctions();
  // The launch-time rendezvous reuse setting is bundled with the kernel, so we
  // need to include it in the cache key.
  cache_key = FingerprintCat128(cache_key, reuse_rendezvous_for_functions);

  for (int i = 0, end = input_dev_ptrs.size(); i < end; ++i) {
    cache_key =
        FingerprintCat128(cache_key, Fingerprint128(input_dev_ptrs[i]->name()));

    auto input_resource = input_resource_variable_dtypes_and_shapes.find(i);
    if (input_resource != input_resource_variable_dtypes_and_shapes.end()) {
      // const DtypeAndPartialTensorShape& dtype_and_shape
      const DtypeAndPartialTensorShape& dtype_and_shape =
          input_resource->second;
      // Add _Arg index, dtype and shape to "cache_key".
      cache_key = FingerprintCat128(cache_key, i);
      cache_key = FingerprintCat128(cache_key, dtype_and_shape.dtype);
      AppendTensorShapeToFingerprint(dtype_and_shape.shape, &cache_key);
    }
  }

  return cache_key;
}