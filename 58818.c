MODRET set_maxconnectsperhost(cmd_rec *cmd) {
  int max;
  config_rec *c;

  if (cmd->argc < 2 || cmd->argc > 3)
    CONF_ERROR(cmd, "wrong number of parameters");

  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  if (strcasecmp(cmd->argv[1], "none") == 0)
    max = 0;

  else {
    char *tmp = NULL;

    max = (int) strtol(cmd->argv[1], &tmp, 10);

    if ((tmp && *tmp) || max < 1)
      CONF_ERROR(cmd, "parameter must be 'none' or a number greater than 0");
  }

  if (cmd->argc == 3) {
    c = add_config_param(cmd->argv[0], 2, NULL, NULL);
    c->argv[1] = pstrdup(c->pool, cmd->argv[2]);

  } else
    c = add_config_param(cmd->argv[0], 1, NULL);

  c->argv[0] = pcalloc(c->pool, sizeof(unsigned int));
  *((unsigned int *) c->argv[0]) = max;

  return PR_HANDLED(cmd);
}
