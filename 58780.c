MODRET auth_pre_pass(cmd_rec *cmd) {
  const char *user;
  char *displaylogin;

  pr_auth_endpwent(cmd->tmp_pool);
  pr_auth_endgrent(cmd->tmp_pool);

  /* Handle cases where PASS might be sent before USER. */
  user = pr_table_get(session.notes, "mod_auth.orig-user", NULL);
  if (user != NULL) {
    config_rec *c;

    c = find_config(main_server->conf, CONF_PARAM, "AllowEmptyPasswords",
      FALSE);
    if (c == NULL) {
      const char *anon_user;
      config_rec *anon_config;

      /* Since we have not authenticated yet, we cannot use the TOPLEVEL_CONF
       * macro to handle <Anonymous> sections.  So we do it manually.
       */
      anon_user = pstrdup(cmd->tmp_pool, user);
      anon_config = pr_auth_get_anon_config(cmd->tmp_pool, &anon_user, NULL,
        NULL);
      if (anon_config != NULL) {
        c = find_config(anon_config->subset, CONF_PARAM, "AllowEmptyPasswords",
          FALSE);
      }
    }
 
    if (c != NULL) {
      int allow_empty_passwords;

      allow_empty_passwords = *((int *) c->argv[0]);
      if (allow_empty_passwords == FALSE) {
        size_t passwd_len = 0;
 
        if (cmd->argc > 1) {
          if (cmd->arg != NULL) {
            passwd_len = strlen(cmd->arg);
          }
        }

        /* Make sure to NOT enforce 'AllowEmptyPasswords off' if e.g.
         * the AllowDotLogin TLSOption is in effect.
         */
        if (cmd->argc == 1 ||
            passwd_len == 0) {

          if (session.auth_mech == NULL ||
              strcmp(session.auth_mech, "mod_tls.c") != 0) {
            pr_log_debug(DEBUG5,
              "Refusing empty password from user '%s' (AllowEmptyPasswords "
              "false)", user);
            pr_log_auth(PR_LOG_NOTICE,
              "Refusing empty password from user '%s'", user);

            pr_event_generate("mod_auth.empty-password", user);
            pr_response_add_err(R_501, _("Login incorrect."));
            return PR_ERROR(cmd);
          }

          pr_log_debug(DEBUG9, "%s", "'AllowEmptyPasswords off' in effect, "
            "BUT client authenticated via the AllowDotLogin TLSOption");
        }
      }
    }
  }

  /* Look for a DisplayLogin file which has an absolute path.  If we find one,
   * open a filehandle, such that that file can be displayed even if the
   * session is chrooted.  DisplayLogin files with relative paths will be
   * handled after chroot, preserving the old behavior.
   */

  displaylogin = get_param_ptr(TOPLEVEL_CONF, "DisplayLogin", FALSE);
  if (displaylogin &&
      *displaylogin == '/') {
    struct stat st;

    displaylogin_fh = pr_fsio_open(displaylogin, O_RDONLY);
    if (displaylogin_fh == NULL) {
      pr_log_debug(DEBUG6, "unable to open DisplayLogin file '%s': %s",
        displaylogin, strerror(errno));

    } else {
      if (pr_fsio_fstat(displaylogin_fh, &st) < 0) {
        pr_log_debug(DEBUG6, "unable to stat DisplayLogin file '%s': %s",
          displaylogin, strerror(errno));
        pr_fsio_close(displaylogin_fh);
        displaylogin_fh = NULL;

      } else {
        if (S_ISDIR(st.st_mode)) {
          errno = EISDIR;
          pr_log_debug(DEBUG6, "unable to use DisplayLogin file '%s': %s",
            displaylogin, strerror(errno));
          pr_fsio_close(displaylogin_fh);
          displaylogin_fh = NULL;
        }
      }
    }
  }

  return PR_DECLINED(cmd);
}
