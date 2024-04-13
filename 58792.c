static int have_client_limits(cmd_rec *cmd) {
  if (find_config(TOPLEVEL_CONF, CONF_PARAM, "MaxClientsPerClass", FALSE) != NULL) {
    return TRUE;
  }

  if (find_config(TOPLEVEL_CONF, CONF_PARAM, "MaxClientsPerHost", FALSE) != NULL) {
    return TRUE;
  }

  if (find_config(TOPLEVEL_CONF, CONF_PARAM, "MaxClientsPerUser", FALSE) != NULL) {
    return TRUE;
  }

  if (find_config(TOPLEVEL_CONF, CONF_PARAM, "MaxClients", FALSE) != NULL) {
    return TRUE;
  }

  if (find_config(TOPLEVEL_CONF, CONF_PARAM, "MaxHostsPerUser", FALSE) != NULL) {
    return TRUE;
  }

  return FALSE;
}
