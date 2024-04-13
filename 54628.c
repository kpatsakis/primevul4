static int csnmp_config_add_data_instance(data_definition_t *dd,
                                          oconfig_item_t *ci) {
  char buffer[DATA_MAX_NAME_LEN];
  int status;

  status = cf_util_get_string_buffer(ci, buffer, sizeof(buffer));
  if (status != 0)
    return status;

  if (dd->is_table) {
    /* Instance is an OID */
    dd->instance.oid.oid_len = MAX_OID_LEN;

    if (!read_objid(buffer, dd->instance.oid.oid, &dd->instance.oid.oid_len)) {
      ERROR("snmp plugin: read_objid (%s) failed.", buffer);
      return (-1);
    }
  } else {
    /* Instance is a simple string */
    sstrncpy(dd->instance.string, buffer, sizeof(dd->instance.string));
  }

  return (0);
} /* int csnmp_config_add_data_instance */
