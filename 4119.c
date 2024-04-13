Fprint128 GetDeviceCacheKey(EagerOperation* op, const EagerContext& ctx) {
  Fprint128 device_cache_key = op->MutableAttrs()->CacheKey(op->DeviceName());
  device_cache_key =
      FingerprintCat128(device_cache_key, ctx.AllowSoftPlacement());
  return device_cache_key;
}