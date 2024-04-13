MODRET auth_log_pass(cmd_rec *cmd) {

  /* Only log, to the syslog, that the login has succeeded here, where we
   * know that the login has definitely succeeded.
   */
  pr_log_auth(PR_LOG_INFO, "%s %s: Login successful.",
    (session.anon_config != NULL) ? "ANON" : C_USER, session.user);

  if (cmd->arg != NULL) {
    size_t passwd_len;

    /* And scrub the memory holding the password sent by the client, for
     * safety/security.
     */
    passwd_len = strlen(cmd->arg);
    pr_memscrub(cmd->arg, passwd_len);
  }

  return PR_DECLINED(cmd);
}
