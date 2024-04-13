static int csnmp_oid_suffix(oid_t *dst, oid_t const *src, oid_t const *root) {
  /* Make sure "src" is in "root"s subtree. */
  if (src->oid_len <= root->oid_len)
    return (EINVAL);
  if (snmp_oid_ncompare(root->oid, root->oid_len, src->oid, src->oid_len,
                        /* n = */ root->oid_len) != 0)
    return (EINVAL);

  memset(dst, 0, sizeof(*dst));
  dst->oid_len = src->oid_len - root->oid_len;
  memcpy(dst->oid, &src->oid[root->oid_len],
         dst->oid_len * sizeof(dst->oid[0]));
  return (0);
}
