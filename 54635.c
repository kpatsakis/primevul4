static void csnmp_host_close_session(host_definition_t *host) /* {{{ */
{
  if (host->sess_handle == NULL)
    return;

  snmp_sess_close(host->sess_handle);
  host->sess_handle = NULL;
} /* }}} void csnmp_host_close_session */
