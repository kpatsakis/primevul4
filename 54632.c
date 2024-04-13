static int csnmp_config_add_host_priv_protocol(host_definition_t *hd,
                                               oconfig_item_t *ci) {
  char buffer[4];
  int status;

  status = cf_util_get_string_buffer(ci, buffer, sizeof(buffer));
  if (status != 0)
    return status;

  if (strcasecmp("AES", buffer) == 0) {
    hd->priv_protocol = usmAESPrivProtocol;
    hd->priv_protocol_len = sizeof(usmAESPrivProtocol) / sizeof(oid);
  } else if (strcasecmp("DES", buffer) == 0) {
    hd->priv_protocol = usmDESPrivProtocol;
    hd->priv_protocol_len = sizeof(usmDESPrivProtocol) / sizeof(oid);
  } else {
    WARNING("snmp plugin: The `PrivProtocol' config option must be `AES' or "
            "`DES'.");
    return (-1);
  }

  DEBUG("snmp plugin: host = %s; host->priv_protocol = %s;", hd->name,
        hd->priv_protocol == usmAESPrivProtocol ? "AES" : "DES");

  return (0);
} /* int csnmp_config_add_host_priv_protocol */
