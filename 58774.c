static int auth_init(void) {
  /* Add the commands handled by this module to the HELP list. */ 
  pr_help_add(C_USER, _("<sp> username"), TRUE);
  pr_help_add(C_PASS, _("<sp> password"), TRUE);
  pr_help_add(C_ACCT, _("is not implemented"), FALSE);
  pr_help_add(C_REIN, _("is not implemented"), FALSE);

  /* By default, enable auth checking */
  set_auth_check(auth_cmd_chk_cb);

  return 0;
}
