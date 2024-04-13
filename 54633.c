static int csnmp_config_add_host_security_level(host_definition_t *hd,
                                                oconfig_item_t *ci) {
  char buffer[16];
  int status;

  status = cf_util_get_string_buffer(ci, buffer, sizeof(buffer));
  if (status != 0)
    return status;

  if (strcasecmp("noAuthNoPriv", buffer) == 0)
    hd->security_level = SNMP_SEC_LEVEL_NOAUTH;
  else if (strcasecmp("authNoPriv", buffer) == 0)
    hd->security_level = SNMP_SEC_LEVEL_AUTHNOPRIV;
  else if (strcasecmp("authPriv", buffer) == 0)
    hd->security_level = SNMP_SEC_LEVEL_AUTHPRIV;
  else {
    WARNING("snmp plugin: The `SecurityLevel' config option must be "
            "`noAuthNoPriv', `authNoPriv', or `authPriv'.");
    return (-1);
  }

  DEBUG("snmp plugin: host = %s; host->security_level = %d;", hd->name,
        hd->security_level);

  return (0);
} /* int csnmp_config_add_host_security_level */
