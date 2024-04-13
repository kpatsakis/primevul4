static int csnmp_oid_to_string(char *buffer, size_t buffer_size,
                               oid_t const *o) {
  char oid_str[MAX_OID_LEN][16];
  char *oid_str_ptr[MAX_OID_LEN];

  for (size_t i = 0; i < o->oid_len; i++) {
    ssnprintf(oid_str[i], sizeof(oid_str[i]), "%lu", (unsigned long)o->oid[i]);
    oid_str_ptr[i] = oid_str[i];
  }

  return (strjoin(buffer, buffer_size, oid_str_ptr, o->oid_len,
                  /* separator = */ "."));
}
