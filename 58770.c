static int auth_count_scoreboard(cmd_rec *cmd, const char *user) {
  char *key;
  void *v;
  pr_scoreboard_entry_t *score = NULL;
  long cur = 0, hcur = 0, ccur = 0, hostsperuser = 1, usersessions = 0;
  config_rec *c = NULL, *maxc = NULL;

  /* First, check to see which Max* directives are configured.  If none
   * are configured, then there is no need for us to needlessly scan the
   * ScoreboardFile.
   */
  if (have_client_limits(cmd) == FALSE) {
    return 0;
  }

  /* Determine how many users are currently connected. */

  /* We use this call to get the possibly-changed user name. */
  c = pr_auth_get_anon_config(cmd->tmp_pool, &user, NULL, NULL);

  /* Gather our statistics. */
  if (user != NULL) {
    char curr_server_addr[80] = {'\0'};

    snprintf(curr_server_addr, sizeof(curr_server_addr), "%s:%d",
      pr_netaddr_get_ipstr(session.c->local_addr), main_server->ServerPort);
    curr_server_addr[sizeof(curr_server_addr)-1] = '\0';

    if (pr_rewind_scoreboard() < 0) {
      pr_log_pri(PR_LOG_NOTICE, "error rewinding scoreboard: %s",
        strerror(errno));
    }

    while ((score = pr_scoreboard_entry_read()) != NULL) {
      unsigned char same_host = FALSE;

      pr_signals_handle();

      /* Make sure it matches our current server. */
      if (strcmp(score->sce_server_addr, curr_server_addr) == 0) {

        if ((c != NULL && c->config_type == CONF_ANON &&
            !strcmp(score->sce_user, user)) || c == NULL) {

          /* This small hack makes sure that cur is incremented properly
           * when dealing with anonymous logins (the timing of anonymous
           * login updates to the scoreboard makes this...odd).
           */
          if (c != NULL &&
              c->config_type == CONF_ANON &&
              cur == 0) {
              cur = 1;
          }

          /* Only count authenticated clients, as per the documentation. */
          if (strncmp(score->sce_user, "(none)", 7) == 0) {
            continue;
          }

          cur++;

          /* Count up sessions on a per-host basis. */

          if (!strcmp(score->sce_client_addr,
              pr_netaddr_get_ipstr(session.c->remote_addr))) {
            same_host = TRUE;

            /* This small hack makes sure that hcur is incremented properly
             * when dealing with anonymous logins (the timing of anonymous
             * login updates to the scoreboard makes this...odd).
             */
            if (c != NULL &&
                c->config_type == CONF_ANON &&
                hcur == 0) {
              hcur = 1;
            }

            hcur++;
          }

          /* Take a per-user count of connections. */
          if (strcmp(score->sce_user, user) == 0) {
            usersessions++;

            /* Count up unique hosts. */
            if (!same_host) {
              hostsperuser++;
            }
          }
        }

        if (session.conn_class != NULL &&
            strcasecmp(score->sce_class, session.conn_class->cls_name) == 0) {
          ccur++;
        }
      }
    }
    pr_restore_scoreboard();
    PRIVS_RELINQUISH
  }

  key = "client-count";
  (void) pr_table_remove(session.notes, key, NULL);
  v = palloc(session.pool, sizeof(unsigned int));
  *((unsigned int *) v) = cur;

  if (pr_table_add(session.notes, key, v, sizeof(unsigned int)) < 0) {
    if (errno != EEXIST) {
      pr_log_pri(PR_LOG_WARNING,
        "warning: error stashing '%s': %s", key, strerror(errno));
    }
  }

  if (session.conn_class != NULL) {
    key = "class-client-count";
    (void) pr_table_remove(session.notes, key, NULL);
    v = palloc(session.pool, sizeof(unsigned int));
    *((unsigned int *) v) = ccur;

    if (pr_table_add(session.notes, key, v, sizeof(unsigned int)) < 0) {
      if (errno != EEXIST) {
        pr_log_pri(PR_LOG_WARNING,
          "warning: error stashing '%s': %s", key, strerror(errno));
      }
    }
  }

  /* Try to determine what MaxClients/MaxHosts limits apply to this session
   * (if any) and count through the runtime file to see if this limit would
   * be exceeded.
   */

  maxc = find_config(cmd->server->conf, CONF_PARAM, "MaxClientsPerClass",
    FALSE);
  while (session.conn_class != NULL && maxc) {
    char *maxstr = "Sorry, the maximum number of clients (%m) from your class "
      "are already connected.";
    unsigned int *max = maxc->argv[1];

    if (strcmp(maxc->argv[0], session.conn_class->cls_name) != 0) {
      maxc = find_config_next(maxc, maxc->next, CONF_PARAM,
        "MaxClientsPerClass", FALSE);
      continue;
    }

    if (maxc->argc > 2) {
      maxstr = maxc->argv[2];
    }

    if (*max &&
        ccur > *max) {
      char maxn[20] = {'\0'};

      pr_event_generate("mod_auth.max-clients-per-class",
        session.conn_class->cls_name);

      snprintf(maxn, sizeof(maxn), "%u", *max);
      pr_response_send(R_530, "%s", sreplace(cmd->tmp_pool, maxstr, "%m", maxn,
        NULL));
      (void) pr_cmd_dispatch_phase(cmd, LOG_CMD_ERR, 0);

      pr_log_auth(PR_LOG_NOTICE,
        "Connection refused (MaxClientsPerClass %s %u)",
        session.conn_class->cls_name, *max);
      pr_session_disconnect(&auth_module, PR_SESS_DISCONNECT_CONFIG_ACL,
        "Denied by MaxClientsPerClass");
    }

    break;
  }

  maxc = find_config(TOPLEVEL_CONF, CONF_PARAM, "MaxClientsPerHost", FALSE);
  if (maxc) {
    char *maxstr = "Sorry, the maximum number of clients (%m) from your host "
      "are already connected.";
    unsigned int *max = maxc->argv[0];

    if (maxc->argc > 1) {
      maxstr = maxc->argv[1];
    }

    if (*max && hcur > *max) {
      char maxn[20] = {'\0'};

      pr_event_generate("mod_auth.max-clients-per-host", session.c);

      snprintf(maxn, sizeof(maxn), "%u", *max);
      pr_response_send(R_530, "%s", sreplace(cmd->tmp_pool, maxstr, "%m", maxn,
        NULL));
      (void) pr_cmd_dispatch_phase(cmd, LOG_CMD_ERR, 0);

      pr_log_auth(PR_LOG_NOTICE,
        "Connection refused (MaxClientsPerHost %u)", *max);
      pr_session_disconnect(&auth_module, PR_SESS_DISCONNECT_CONFIG_ACL,
        "Denied by MaxClientsPerHost");
    }
  }

  /* Check for any configured MaxClientsPerUser. */
  maxc = find_config(TOPLEVEL_CONF, CONF_PARAM, "MaxClientsPerUser", FALSE);
  if (maxc) {
    char *maxstr = "Sorry, the maximum number of clients (%m) for this user "
      "are already connected.";
    unsigned int *max = maxc->argv[0];

    if (maxc->argc > 1) {
      maxstr = maxc->argv[1];
    }

    if (*max && usersessions > *max) {
      char maxn[20] = {'\0'};

      pr_event_generate("mod_auth.max-clients-per-user", user);

      snprintf(maxn, sizeof(maxn), "%u", *max);
      pr_response_send(R_530, "%s", sreplace(cmd->tmp_pool, maxstr, "%m", maxn,
        NULL));
      (void) pr_cmd_dispatch_phase(cmd, LOG_CMD_ERR, 0);

      pr_log_auth(PR_LOG_NOTICE,
        "Connection refused (MaxClientsPerUser %u)", *max);
      pr_session_disconnect(&auth_module, PR_SESS_DISCONNECT_CONFIG_ACL,
        "Denied by MaxClientsPerUser");
    }
  }

  maxc = find_config(TOPLEVEL_CONF, CONF_PARAM, "MaxClients", FALSE);
  if (maxc) {
    char *maxstr = "Sorry, the maximum number of allowed clients (%m) are "
      "already connected.";
    unsigned int *max = maxc->argv[0];

    if (maxc->argc > 1) {
      maxstr = maxc->argv[1];
    }

    if (*max && cur > *max) {
      char maxn[20] = {'\0'};

      pr_event_generate("mod_auth.max-clients", NULL);

      snprintf(maxn, sizeof(maxn), "%u", *max);
      pr_response_send(R_530, "%s", sreplace(cmd->tmp_pool, maxstr, "%m", maxn,
        NULL));
      (void) pr_cmd_dispatch_phase(cmd, LOG_CMD_ERR, 0);

      pr_log_auth(PR_LOG_NOTICE, "Connection refused (MaxClients %u)", *max);
      pr_session_disconnect(&auth_module, PR_SESS_DISCONNECT_CONFIG_ACL,
        "Denied by MaxClients");
    }
  }

  maxc = find_config(TOPLEVEL_CONF, CONF_PARAM, "MaxHostsPerUser", FALSE);
  if (maxc) {
    char *maxstr = "Sorry, the maximum number of hosts (%m) for this user are "
      "already connected.";
    unsigned int *max = maxc->argv[0];

    if (maxc->argc > 1) {
      maxstr = maxc->argv[1];
    }

    if (*max && hostsperuser > *max) {
      char maxn[20] = {'\0'};

      pr_event_generate("mod_auth.max-hosts-per-user", user);

      snprintf(maxn, sizeof(maxn), "%u", *max);
      pr_response_send(R_530, "%s", sreplace(cmd->tmp_pool, maxstr, "%m", maxn,
        NULL));
      (void) pr_cmd_dispatch_phase(cmd, LOG_CMD_ERR, 0);

      pr_log_auth(PR_LOG_NOTICE, "Connection refused (MaxHostsPerUser %u)",
        *max);
      pr_session_disconnect(&auth_module, PR_SESS_DISCONNECT_CONFIG_ACL,
        "Denied by MaxHostsPerUser");
    }
  }

  return 0;
}
