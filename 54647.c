static value_t csnmp_value_list_to_value(struct variable_list *vl, int type,
                                         double scale, double shift,
                                         const char *host_name,
                                         const char *data_name) {
  value_t ret;
  uint64_t tmp_unsigned = 0;
  int64_t tmp_signed = 0;
  _Bool defined = 1;
  /* Set to true when the original SNMP type appears to have been signed. */
  _Bool prefer_signed = 0;

  if ((vl->type == ASN_INTEGER) || (vl->type == ASN_UINTEGER) ||
      (vl->type == ASN_COUNTER)
#ifdef ASN_TIMETICKS
      || (vl->type == ASN_TIMETICKS)
#endif
      || (vl->type == ASN_GAUGE)) {
    tmp_unsigned = (uint32_t)*vl->val.integer;
    tmp_signed = (int32_t)*vl->val.integer;

    if (vl->type == ASN_INTEGER)
      prefer_signed = 1;

    DEBUG("snmp plugin: Parsed int32 value is %" PRIu64 ".", tmp_unsigned);
  } else if (vl->type == ASN_COUNTER64) {
    tmp_unsigned = (uint32_t)vl->val.counter64->high;
    tmp_unsigned = tmp_unsigned << 32;
    tmp_unsigned += (uint32_t)vl->val.counter64->low;
    tmp_signed = (int64_t)tmp_unsigned;
    DEBUG("snmp plugin: Parsed int64 value is %" PRIu64 ".", tmp_unsigned);
  } else if (vl->type == ASN_OCTET_STR) {
    /* We'll handle this later.. */
  } else {
    char oid_buffer[1024] = {0};

    snprint_objid(oid_buffer, sizeof(oid_buffer) - 1, vl->name,
                  vl->name_length);

#ifdef ASN_NULL
    if (vl->type == ASN_NULL)
      INFO("snmp plugin: OID \"%s\" is undefined (type ASN_NULL)", oid_buffer);
    else
#endif
      WARNING("snmp plugin: I don't know the ASN type #%i "
              "(OID: \"%s\", data block \"%s\", host block \"%s\")",
              (int)vl->type, oid_buffer,
              (data_name != NULL) ? data_name : "UNKNOWN",
              (host_name != NULL) ? host_name : "UNKNOWN");

    defined = 0;
  }

  if (vl->type == ASN_OCTET_STR) {
    int status = -1;

    if (vl->val.string != NULL) {
      char string[64];
      size_t string_length;

      string_length = sizeof(string) - 1;
      if (vl->val_len < string_length)
        string_length = vl->val_len;

      /* The strings we get from the Net-SNMP library may not be null
       * terminated. That is why we're using `memcpy' here and not `strcpy'.
       * `string_length' is set to `vl->val_len' which holds the length of the
       * string.  -octo */
      memcpy(string, vl->val.string, string_length);
      string[string_length] = 0;

      status = parse_value(string, &ret, type);
      if (status != 0) {
        ERROR("snmp plugin: host %s: csnmp_value_list_to_value: Parsing string "
              "as %s failed: %s",
              (host_name != NULL) ? host_name : "UNKNOWN",
              DS_TYPE_TO_STRING(type), string);
      }
    }

    if (status != 0) {
      switch (type) {
      case DS_TYPE_COUNTER:
      case DS_TYPE_DERIVE:
      case DS_TYPE_ABSOLUTE:
        memset(&ret, 0, sizeof(ret));
        break;

      case DS_TYPE_GAUGE:
        ret.gauge = NAN;
        break;

      default:
        ERROR("snmp plugin: csnmp_value_list_to_value: Unknown "
              "data source type: %i.",
              type);
        ret.gauge = NAN;
      }
    }
  } /* if (vl->type == ASN_OCTET_STR) */
  else if (type == DS_TYPE_COUNTER) {
    ret.counter = tmp_unsigned;
  } else if (type == DS_TYPE_GAUGE) {
    if (!defined)
      ret.gauge = NAN;
    else if (prefer_signed)
      ret.gauge = (scale * tmp_signed) + shift;
    else
      ret.gauge = (scale * tmp_unsigned) + shift;
  } else if (type == DS_TYPE_DERIVE) {
    if (prefer_signed)
      ret.derive = (derive_t)tmp_signed;
    else
      ret.derive = (derive_t)tmp_unsigned;
  } else if (type == DS_TYPE_ABSOLUTE) {
    ret.absolute = (absolute_t)tmp_unsigned;
  } else {
    ERROR("snmp plugin: csnmp_value_list_to_value: Unknown data source "
          "type: %i.",
          type);
    ret.gauge = NAN;
  }

  return (ret);
} /* value_t csnmp_value_list_to_value */
