static void csnmp_oid_init(oid_t *dst, oid const *src, size_t n) {
  assert(n <= STATIC_ARRAY_SIZE(dst->oid));
  memcpy(dst->oid, src, sizeof(*src) * n);
  dst->oid_len = n;
}
