MODRET auth_user(cmd_rec *cmd) {
  int nopass = FALSE;
  config_rec *c;
  const char *denymsg = NULL, *user, *origuser;
  int failnopwprompt = 0, aclp, i;
  unsigned char *anon_require_passwd = NULL, *login_passwd_prompt = NULL;

  if (cmd->argc < 2) {
    return PR_ERROR_MSG(cmd, R_500, _("USER: command requires a parameter"));
  }

  if (logged_in) {
    /* If the client has already authenticated, BUT the given USER command
     * here is for the exact same user name, then allow the command to
     * succeed (Bug#4217).
     */
    origuser = pr_table_get(session.notes, "mod_auth.orig-user", NULL);
    if (origuser != NULL &&
        strcmp(origuser, cmd->arg) == 0) {
      pr_response_add(R_230, _("User %s logged in"), origuser);
      return PR_HANDLED(cmd);
    }

    pr_response_add_err(R_501, "%s", _("Reauthentication not supported"));
    return PR_ERROR(cmd);
  }

  user = cmd->arg;

  (void) pr_table_remove(session.notes, "mod_auth.orig-user", NULL);
  (void) pr_table_remove(session.notes, "mod_auth.anon-passwd", NULL);

  if (pr_table_add_dup(session.notes, "mod_auth.orig-user", user, 0) < 0) {
    pr_log_debug(DEBUG3, "error stashing 'mod_auth.orig-user' in "
      "session.notes: %s", strerror(errno));
  }

  origuser = user;
  c = pr_auth_get_anon_config(cmd->tmp_pool, &user, NULL, NULL);

  /* Check for AccessDenyMsg */
  denymsg = get_param_ptr((c ? c->subset : cmd->server->conf), "AccessDenyMsg",
    FALSE);
  if (denymsg != NULL) {
    if (strstr(denymsg, "%u") != NULL) {
      denymsg = sreplace(cmd->tmp_pool, denymsg, "%u", user, NULL);
    }
  }

  login_passwd_prompt = get_param_ptr(
    (c && c->config_type == CONF_ANON) ? c->subset : main_server->conf,
    "LoginPasswordPrompt", FALSE);

  if (login_passwd_prompt &&
      *login_passwd_prompt == FALSE) {
    failnopwprompt = TRUE;

  } else {
    failnopwprompt = FALSE;
  }

  if (failnopwprompt) {
    if (!user) {
      (void) pr_table_remove(session.notes, "mod_auth.orig-user", NULL);
      (void) pr_table_remove(session.notes, "mod_auth.anon-passwd", NULL);

      pr_log_pri(PR_LOG_NOTICE, "USER %s (Login failed): Not a UserAlias",
        origuser);

      if (denymsg) {
        pr_response_send(R_530, "%s", denymsg);

      } else {
        pr_response_send(R_530, _("Login incorrect."));
      }

      pr_session_end(0);
    }

    aclp = login_check_limits(main_server->conf, FALSE, TRUE, &i);

    if (c && c->config_type != CONF_ANON) {
      c = (config_rec *) pcalloc(session.pool, sizeof(config_rec));
      c->config_type = CONF_ANON;
      c->name = "";	/* don't really need this yet */
      c->subset = main_server->conf;
    }

    if (c) {
      if (!login_check_limits(c->subset, FALSE, TRUE, &i) ||
          (!aclp && !i) ) {
        (void) pr_table_remove(session.notes, "mod_auth.orig-user", NULL);
        (void) pr_table_remove(session.notes, "mod_auth.anon-passwd", NULL);

        pr_log_auth(PR_LOG_NOTICE, "ANON %s: Limit access denies login",
          origuser);

        if (denymsg) {
          pr_response_send(R_530, "%s", denymsg);

        } else {
          pr_response_send(R_530, _("Login incorrect."));
        }

        pr_session_disconnect(&auth_module, PR_SESS_DISCONNECT_CONFIG_ACL,
          "Denied by <Limit LOGIN>");
      }
    }

    if (c == NULL &&
        aclp == 0) {
      (void) pr_table_remove(session.notes, "mod_auth.orig-user", NULL);
      (void) pr_table_remove(session.notes, "mod_auth.anon-passwd", NULL);

      pr_log_auth(PR_LOG_NOTICE,
        "USER %s: Limit access denies login", origuser);

      if (denymsg) {
        pr_response_send(R_530, "%s", denymsg);

      } else {
        pr_response_send(R_530, "%s", _("Login incorrect."));
      }

      pr_session_disconnect(&auth_module, PR_SESS_DISCONNECT_CONFIG_ACL,
        "Denied by <Limit LOGIN>");
    }
  }

  if (c)
    anon_require_passwd = get_param_ptr(c->subset, "AnonRequirePassword",
      FALSE);

  if (c && user && (!anon_require_passwd || *anon_require_passwd == FALSE))
    nopass = TRUE;

  session.gids = NULL;
  session.groups = NULL;
  session.user = NULL;
  session.group = NULL;

  if (nopass) {
    pr_response_add(R_331, _("Anonymous login ok, send your complete email "
      "address as your password"));

  } else if (pr_auth_requires_pass(cmd->tmp_pool, user) == FALSE) {
    /* Check to see if a password from the client is required.  In the
     * vast majority of cases, a password will be required.
     */

    /* Act as if we received a PASS command from the client. */
    cmd_rec *fakecmd = pr_cmd_alloc(cmd->pool, 2, NULL);

    /* We use pstrdup() here, rather than assigning C_PASS directly, since
     * code elsewhere will attempt to modify this buffer, and C_PASS is
     * a string literal.
     */
    fakecmd->argv[0] = pstrdup(fakecmd->pool, C_PASS);
    fakecmd->argv[1] = NULL;
    fakecmd->arg = NULL;

    c = add_config_param_set(&cmd->server->conf, "authenticated", 1, NULL);
    c->argv[0] = pcalloc(c->pool, sizeof(unsigned char));
    *((unsigned char *) c->argv[0]) = TRUE;

    authenticated_without_pass = TRUE;
    pr_log_auth(PR_LOG_NOTICE, "USER %s: Authenticated without password", user);

    pr_cmd_dispatch(fakecmd);

  } else {
    pr_response_add(R_331, _("Password required for %s"),
      (char *) cmd->argv[1]);
  }

  return PR_HANDLED(cmd);
}
