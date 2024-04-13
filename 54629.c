static int csnmp_config_add_data_values(data_definition_t *dd,
                                        oconfig_item_t *ci) {
  if (ci->values_num < 1) {
    WARNING("snmp plugin: `Values' needs at least one argument.");
    return (-1);
  }

  for (int i = 0; i < ci->values_num; i++)
    if (ci->values[i].type != OCONFIG_TYPE_STRING) {
      WARNING("snmp plugin: `Values' needs only string argument.");
      return (-1);
    }

  sfree(dd->values);
  dd->values_len = 0;
  dd->values = malloc(sizeof(*dd->values) * ci->values_num);
  if (dd->values == NULL)
    return (-1);
  dd->values_len = (size_t)ci->values_num;

  for (int i = 0; i < ci->values_num; i++) {
    dd->values[i].oid_len = MAX_OID_LEN;

    if (NULL == snmp_parse_oid(ci->values[i].value.string, dd->values[i].oid,
                               &dd->values[i].oid_len)) {
      ERROR("snmp plugin: snmp_parse_oid (%s) failed.",
            ci->values[i].value.string);
      free(dd->values);
      dd->values = NULL;
      dd->values_len = 0;
      return (-1);
    }
  }

  return (0);
} /* int csnmp_config_add_data_instance */
