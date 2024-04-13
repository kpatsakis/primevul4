static int csnmp_config_add_host_auth_protocol(host_definition_t *hd,
                                               oconfig_item_t *ci) {
  char buffer[4];
  int status;

  status = cf_util_get_string_buffer(ci, buffer, sizeof(buffer));
  if (status != 0)
    return status;

  if (strcasecmp("MD5", buffer) == 0) {
    hd->auth_protocol = usmHMACMD5AuthProtocol;
    hd->auth_protocol_len = sizeof(usmHMACMD5AuthProtocol) / sizeof(oid);
  } else if (strcasecmp("SHA", buffer) == 0) {
    hd->auth_protocol = usmHMACSHA1AuthProtocol;
    hd->auth_protocol_len = sizeof(usmHMACSHA1AuthProtocol) / sizeof(oid);
  } else {
    WARNING("snmp plugin: The `AuthProtocol' config option must be `MD5' or "
            "`SHA'.");
    return (-1);
  }

  DEBUG("snmp plugin: host = %s; host->auth_protocol = %s;", hd->name,
        hd->auth_protocol == usmHMACMD5AuthProtocol ? "MD5" : "SHA");

  return (0);
} /* int csnmp_config_add_host_auth_protocol */
