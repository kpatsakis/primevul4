static int csnmp_oid_compare(oid_t const *left, oid_t const *right) {
  return (
      snmp_oid_compare(left->oid, left->oid_len, right->oid, right->oid_len));
}
