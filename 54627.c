static int csnmp_config_add_data_blacklist_match_inverted(data_definition_t *dd,
                                                          oconfig_item_t *ci) {
  if ((ci->values_num != 1) || (ci->values[0].type != OCONFIG_TYPE_BOOLEAN)) {
    WARNING("snmp plugin: `InvertMatch' needs exactly one boolean argument.");
    return (-1);
  }

  dd->invert_match = ci->values[0].value.boolean ? 1 : 0;

  return (0);
} /* int csnmp_config_add_data_blacklist_match_inverted */
