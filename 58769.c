static int auth_cmd_chk_cb(cmd_rec *cmd) {
  if (auth_have_authenticated == FALSE) {
    unsigned char *authd;

    authd = get_param_ptr(cmd->server->conf, "authenticated", FALSE);

    if (authd == NULL ||
        *authd == FALSE) {
      pr_response_send(R_530, _("Please login with USER and PASS"));
      return FALSE;
    }

    auth_have_authenticated = TRUE;
  }

  return TRUE;
}
