MODRET auth_pass(cmd_rec *cmd) {
  const char *user = NULL;
  int res = 0;

  if (logged_in) {
    return PR_ERROR_MSG(cmd, R_503, _("You are already logged in"));
  }

  user = pr_table_get(session.notes, "mod_auth.orig-user", NULL);
  if (user == NULL) {
    (void) pr_table_remove(session.notes, "mod_auth.orig-user", NULL);
    (void) pr_table_remove(session.notes, "mod_auth.anon-passwd", NULL);

    return PR_ERROR_MSG(cmd, R_503, _("Login with USER first"));
  }

  /* Clear any potentially cached directory config */
  session.anon_config = NULL;
  session.dir_config = NULL;

  res = setup_env(cmd->tmp_pool, cmd, user, cmd->arg);
  if (res == 1) {
    config_rec *c = NULL;

    c = add_config_param_set(&cmd->server->conf, "authenticated", 1, NULL);
    c->argv[0] = pcalloc(c->pool, sizeof(unsigned char));
    *((unsigned char *) c->argv[0]) = TRUE;

    set_auth_check(NULL);

    (void) pr_table_remove(session.notes, "mod_auth.anon-passwd", NULL);

    if (session.sf_flags & SF_ANON) {
      if (pr_table_add_dup(session.notes, "mod_auth.anon-passwd",
          pr_fs_decode_path(cmd->server->pool, cmd->arg), 0) < 0) {
        pr_log_debug(DEBUG3,
          "error stashing anonymous password in session.notes: %s",
          strerror(errno));
      }
    }

    logged_in = TRUE;

    if (pr_trace_get_level(timing_channel)) {
      unsigned long elapsed_ms;
      uint64_t finish_ms;

      pr_gettimeofday_millis(&finish_ms);
      elapsed_ms = (unsigned long) (finish_ms - session.connect_time_ms);

      pr_trace_msg(timing_channel, 4,
        "Time before successful login (via '%s'): %lu ms", session.auth_mech,
        elapsed_ms);
    }

    return PR_HANDLED(cmd);
  }

  (void) pr_table_remove(session.notes, "mod_auth.anon-passwd", NULL);

  if (res == 0) {
    unsigned int max_logins, *max = NULL;
    const char *denymsg = NULL;

    /* check for AccessDenyMsg */
    if ((denymsg = get_param_ptr((session.anon_config ?
        session.anon_config->subset : cmd->server->conf),
        "AccessDenyMsg", FALSE)) != NULL) {

      if (strstr(denymsg, "%u") != NULL) {
        denymsg = sreplace(cmd->tmp_pool, denymsg, "%u", user, NULL);
      }
    }

    max = get_param_ptr(main_server->conf, "MaxLoginAttempts", FALSE);
    if (max != NULL) {
      max_logins = *max;

    } else {
      max_logins = 3;
    }

    if (max_logins > 0 &&
        ++auth_tries >= max_logins) {
      if (denymsg) {
        pr_response_send(R_530, "%s", denymsg);

      } else {
        pr_response_send(R_530, "%s", _("Login incorrect."));
      }

      pr_log_auth(PR_LOG_NOTICE,
        "Maximum login attempts (%u) exceeded, connection refused", max_logins);

      /* Generate an event about this limit being exceeded. */
      pr_event_generate("mod_auth.max-login-attempts", session.c);

      pr_session_disconnect(&auth_module, PR_SESS_DISCONNECT_CONFIG_ACL,
        "Denied by MaxLoginAttempts");
    }

    return PR_ERROR_MSG(cmd, R_530, denymsg ? denymsg : _("Login incorrect."));
  }

  return PR_HANDLED(cmd);
}
