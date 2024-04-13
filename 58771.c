MODRET auth_err_pass(cmd_rec *cmd) {
  const char *user;

  user = pr_table_get(session.notes, "mod_auth.orig-user", NULL);
  if (user != NULL) {
    login_failed(cmd->tmp_pool, user);
  }

  /* Remove the stashed original USER name here in a LOG_CMD_ERR handler, so
   * that other modules, who may want to lookup the original USER parameter on
   * a failed login in an earlier command handler phase, have a chance to do
   * so.  This removal of the USER parameter on failure was happening directly
   * in the CMD handler previously, thus preventing POST_CMD_ERR handlers from
   * using USER.
   */
  pr_table_remove(session.notes, "mod_auth.orig-user", NULL);

  return PR_HANDLED(cmd);
}
