static int auth_sess_init(void) {
  config_rec *c = NULL;
  unsigned char *tmp = NULL;

  pr_event_register(&auth_module, "core.session-reinit", auth_sess_reinit_ev,
    NULL);

  /* Check for any MaxPasswordSize. */
  c = find_config(main_server->conf, CONF_PARAM, "MaxPasswordSize", FALSE);
  if (c != NULL) {
    size_t len;

    len = *((size_t *) c->argv[0]);
    (void) pr_auth_set_max_password_len(session.pool, len);
  }

  /* Check for a server-specific TimeoutLogin */
  c = find_config(main_server->conf, CONF_PARAM, "TimeoutLogin", FALSE);
  if (c != NULL) {
    TimeoutLogin = *((int *) c->argv[0]);
  }

  /* Start the login timer */
  if (TimeoutLogin) {
    pr_timer_remove(PR_TIMER_LOGIN, &auth_module);
    pr_timer_add(TimeoutLogin, PR_TIMER_LOGIN, &auth_module,
      auth_login_timeout_cb, "TimeoutLogin");
  }

  if (auth_client_connected == FALSE) {
    int res = 0;

    PRIVS_ROOT
    res = pr_open_scoreboard(O_RDWR);
    PRIVS_RELINQUISH

    if (res < 0) {
      switch (res) {
        case PR_SCORE_ERR_BAD_MAGIC:
          pr_log_debug(DEBUG0, "error opening scoreboard: bad/corrupted file");
          break;

        case PR_SCORE_ERR_OLDER_VERSION:
          pr_log_debug(DEBUG0,
            "error opening scoreboard: bad version (too old)");
          break;

        case PR_SCORE_ERR_NEWER_VERSION:
          pr_log_debug(DEBUG0,
            "error opening scoreboard: bad version (too new)");
          break;

        default:
          pr_log_debug(DEBUG0, "error opening scoreboard: %s", strerror(errno));
          break;
      }
    }
  }

  pr_event_register(&auth_module, "core.exit", auth_exit_ev, NULL);

  if (auth_client_connected == FALSE) {
    /* Create an entry in the scoreboard for this session, if we don't already
     * have one.
     */
    if (pr_scoreboard_entry_get(PR_SCORE_CLIENT_ADDR) == NULL) {
      if (pr_scoreboard_entry_add() < 0) {
        pr_log_pri(PR_LOG_NOTICE, "notice: unable to add scoreboard entry: %s",
          strerror(errno));
      }

      pr_scoreboard_entry_update(session.pid,
        PR_SCORE_USER, "(none)",
        PR_SCORE_SERVER_PORT, main_server->ServerPort,
        PR_SCORE_SERVER_ADDR, session.c->local_addr, session.c->local_port,
        PR_SCORE_SERVER_LABEL, main_server->ServerName,
        PR_SCORE_CLIENT_ADDR, session.c->remote_addr,
        PR_SCORE_CLIENT_NAME, session.c->remote_name,
        PR_SCORE_CLASS, session.conn_class ? session.conn_class->cls_name : "",
        PR_SCORE_PROTOCOL, "ftp",
        PR_SCORE_BEGIN_SESSION, time(NULL),
        NULL);
    }

  } else {
    /* We're probably handling a HOST comand, and the server changed; just
     * update the SERVER_LABEL field.
     */
    pr_scoreboard_entry_update(session.pid,
      PR_SCORE_SERVER_LABEL, main_server->ServerName,
      NULL);
  }

  /* Should we create the home for a user, if they don't have one? */
  tmp = get_param_ptr(main_server->conf, "CreateHome", FALSE);
  if (tmp != NULL &&
      *tmp == TRUE) {
    mkhome = TRUE;

  } else {
    mkhome = FALSE;
  }

#ifdef PR_USE_LASTLOG
  /* Use the lastlog file, if supported and requested. */
  tmp = get_param_ptr(main_server->conf, "UseLastlog", FALSE);
  if (tmp &&
      *tmp == TRUE) {
    lastlog = TRUE;

  } else {
    lastlog = FALSE;
  }
#endif /* PR_USE_LASTLOG */

  /* Scan the scoreboard now, in order to tally up certain values for
   * substituting in any of the Display* file variables.  This function
   * also performs the MaxConnectionsPerHost enforcement.
   */
  auth_scan_scoreboard();

  auth_client_connected = TRUE;
  return 0;
}
