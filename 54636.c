static void csnmp_host_open_session(host_definition_t *host) {
  struct snmp_session sess;
  int error;

  if (host->sess_handle != NULL)
    csnmp_host_close_session(host);

  snmp_sess_init(&sess);
  sess.peername = host->address;
  switch (host->version) {
  case 1:
    sess.version = SNMP_VERSION_1;
    break;
  case 3:
    sess.version = SNMP_VERSION_3;
    break;
  default:
    sess.version = SNMP_VERSION_2c;
    break;
  }

  if (host->version == 3) {
    sess.securityName = host->username;
    sess.securityNameLen = strlen(host->username);
    sess.securityLevel = host->security_level;

    if (sess.securityLevel == SNMP_SEC_LEVEL_AUTHNOPRIV ||
        sess.securityLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
      sess.securityAuthProto = host->auth_protocol;
      sess.securityAuthProtoLen = host->auth_protocol_len;
      sess.securityAuthKeyLen = USM_AUTH_KU_LEN;
      error = generate_Ku(sess.securityAuthProto, sess.securityAuthProtoLen,
                          (u_char *)host->auth_passphrase,
                          strlen(host->auth_passphrase), sess.securityAuthKey,
                          &sess.securityAuthKeyLen);
      if (error != SNMPERR_SUCCESS) {
        ERROR("snmp plugin: host %s: Error generating Ku from auth_passphrase. "
              "(Error %d)",
              host->name, error);
      }
    }

    if (sess.securityLevel == SNMP_SEC_LEVEL_AUTHPRIV) {
      sess.securityPrivProto = host->priv_protocol;
      sess.securityPrivProtoLen = host->priv_protocol_len;
      sess.securityPrivKeyLen = USM_PRIV_KU_LEN;
      error = generate_Ku(sess.securityAuthProto, sess.securityAuthProtoLen,
                          (u_char *)host->priv_passphrase,
                          strlen(host->priv_passphrase), sess.securityPrivKey,
                          &sess.securityPrivKeyLen);
      if (error != SNMPERR_SUCCESS) {
        ERROR("snmp plugin: host %s: Error generating Ku from priv_passphrase. "
              "(Error %d)",
              host->name, error);
      }
    }

    if (host->context != NULL) {
      sess.contextName = host->context;
      sess.contextNameLen = strlen(host->context);
    }
  } else /* SNMPv1/2 "authenticates" with community string */
  {
    sess.community = (u_char *)host->community;
    sess.community_len = strlen(host->community);
  }

  /* snmp_sess_open will copy the `struct snmp_session *'. */
  host->sess_handle = snmp_sess_open(&sess);

  if (host->sess_handle == NULL) {
    char *errstr = NULL;

    snmp_error(&sess, NULL, NULL, &errstr);

    ERROR("snmp plugin: host %s: snmp_sess_open failed: %s", host->name,
          (errstr == NULL) ? "Unknown problem" : errstr);
    sfree(errstr);
  }
} /* void csnmp_host_open_session */
