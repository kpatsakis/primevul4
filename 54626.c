static int csnmp_config_add_data_blacklist(data_definition_t *dd,
                                           oconfig_item_t *ci) {
  if (ci->values_num < 1)
    return (0);

  for (int i = 0; i < ci->values_num; i++) {
    if (ci->values[i].type != OCONFIG_TYPE_STRING) {
      WARNING("snmp plugin: `Ignore' needs only string argument.");
      return (-1);
    }
  }

  dd->ignores_len = 0;
  dd->ignores = NULL;

  for (int i = 0; i < ci->values_num; ++i) {
    if (strarray_add(&(dd->ignores), &(dd->ignores_len),
                     ci->values[i].value.string) != 0) {
      ERROR("snmp plugin: Can't allocate memory");
      strarray_free(dd->ignores, dd->ignores_len);
      return (ENOMEM);
    }
  }
  return 0;
} /* int csnmp_config_add_data_blacklist */
