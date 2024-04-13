MODRET auth_acct(cmd_rec *cmd) {
  pr_response_add(R_502, _("ACCT command not implemented"));
  return PR_HANDLED(cmd);
}
