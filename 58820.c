MODRET set_maxpasswordsize(cmd_rec *cmd) {
  config_rec *c;
  size_t password_len;
  char *len, *ptr = NULL;

  CHECK_ARGS(cmd, 1);
  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  len = cmd->argv[1];
  if (*len == '-') {
    CONF_ERROR(cmd, "badly formatted parameter");
  }

  password_len = strtoul(len, &ptr, 10);
  if (ptr && *ptr) {
    CONF_ERROR(cmd, "badly formatted parameter");
  }

/* XXX Applies to the following modules, which use crypt(3):
 *
 *  mod_ldap (ldap_auth_check; "check" authtab)
 *    ldap_auth_auth ("auth" authtab) calls pr_auth_check()
 *  mod_sql (sql_auth_crypt, via SQLAuthTypes; cmd_check "check" authtab dispatches here)
 *    cmd_auth ("auth" authtab) calls pr_auth_check()
 *  mod_auth_file (authfile_chkpass, "check" authtab)
 *    authfile_auth ("auth" authtab) calls pr_auth_check()
 *  mod_auth_unix (pw_check, "check" authtab)
 *    pw_auth ("auth" authtab) calls pr_auth_check()
 *
 *  mod_sftp uses pr_auth_authenticate(), which will dispatch into above
 *
 *  mod_radius does NOT use either -- up to RADIUS server policy?
 *
 * Is there a common code path that all of the above go through?
 */

  c = add_config_param(cmd->argv[0], 1, NULL);
  c->argv[0] = palloc(c->pool, sizeof(size_t));
  *((size_t *) c->argv[0]) = password_len;

  return PR_HANDLED(cmd);
}
