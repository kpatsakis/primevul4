MODRET auth_rein(cmd_rec *cmd) {
  pr_response_add(R_502, _("REIN command not implemented"));
  return PR_HANDLED(cmd);
}
