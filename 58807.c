MODRET set_allowemptypasswords(cmd_rec *cmd) {
  int allow_empty_passwords = -1;
  config_rec *c = NULL;

  CHECK_ARGS(cmd, 1);
  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL|CONF_ANON);

  allow_empty_passwords = get_boolean(cmd, 1);
  if (allow_empty_passwords == -1) {
    CONF_ERROR(cmd, "expected Boolean parameter");
  }

  c = add_config_param(cmd->argv[0], 1, NULL);
  c->argv[0] = pcalloc(c->pool, sizeof(int));
  *((int *) c->argv[0]) = allow_empty_passwords;
  c->flags |= CF_MERGEDOWN;

  return PR_HANDLED(cmd);
}
