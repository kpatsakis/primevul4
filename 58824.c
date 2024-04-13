MODRET set_rootrevoke(cmd_rec *cmd) {
  int root_revoke = -1;
  config_rec *c = NULL;

  CHECK_ARGS(cmd, 1);
  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL|CONF_ANON);

  /* A RootRevoke value of 0 indicates 'false', 1 indicates 'true', and
   * 2 indicates 'NonCompliantActiveTransfer'.
   */
  root_revoke = get_boolean(cmd, 1);
  if (root_revoke == -1) {
    if (strcasecmp(cmd->argv[1], "UseNonCompliantActiveTransfer") != 0 &&
        strcasecmp(cmd->argv[1], "UseNonCompliantActiveTransfers") != 0) {
      CONF_ERROR(cmd, "expected Boolean parameter");
    }

    root_revoke = 2;
  }

  c = add_config_param(cmd->argv[0], 1, NULL);
  c->argv[0] = pcalloc(c->pool, sizeof(unsigned char));
  *((unsigned char *) c->argv[0]) = (unsigned char) root_revoke;

  c->flags |= CF_MERGEDOWN;
  return PR_HANDLED(cmd);
}
