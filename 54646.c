static int csnmp_strvbcopy(char *dst, /* {{{ */
                           const struct variable_list *vb, size_t dst_size) {
  char *src;
  size_t num_chars;

  if (vb->type == ASN_OCTET_STR)
    src = (char *)vb->val.string;
  else if (vb->type == ASN_BIT_STR)
    src = (char *)vb->val.bitstring;
  else if (vb->type == ASN_IPADDRESS) {
    return ssnprintf(dst, dst_size,
                     "%" PRIu8 ".%" PRIu8 ".%" PRIu8 ".%" PRIu8 "",
                     (uint8_t)vb->val.string[0], (uint8_t)vb->val.string[1],
                     (uint8_t)vb->val.string[2], (uint8_t)vb->val.string[3]);
  } else {
    dst[0] = 0;
    return (EINVAL);
  }

  num_chars = dst_size - 1;
  if (num_chars > vb->val_len)
    num_chars = vb->val_len;

  for (size_t i = 0; i < num_chars; i++) {
    /* Check for control characters. */
    if ((unsigned char)src[i] < 32)
      return (csnmp_strvbcopy_hexstring(dst, vb, dst_size));
    dst[i] = src[i];
  }
  dst[num_chars] = 0;
  dst[dst_size - 1] = 0;

  if (dst_size <= vb->val_len)
    return ENOMEM;

  return 0;
} /* }}} int csnmp_strvbcopy */
