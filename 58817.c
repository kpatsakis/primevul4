MODRET set_maxclientsclass(cmd_rec *cmd) {
  int max;
  config_rec *c;

  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  if (strcasecmp(cmd->argv[2], "none") == 0)
    max = 0;

  else {
    char *endp = NULL;

    max = (int) strtol(cmd->argv[2], &endp, 10);

    if ((endp && *endp) || max < 1)
      CONF_ERROR(cmd, "max must be 'none' or a number greater than 0");
  }

  if (cmd->argc == 4) {
    c = add_config_param(cmd->argv[0], 3, NULL, NULL, NULL);
    c->argv[0] = pstrdup(c->pool, cmd->argv[1]);
    c->argv[1] = pcalloc(c->pool, sizeof(unsigned int));
    *((unsigned int *) c->argv[1]) = max;
    c->argv[2] = pstrdup(c->pool, cmd->argv[3]);

  } else {
    c = add_config_param(cmd->argv[0], 2, NULL, NULL);
    c->argv[0] = pstrdup(c->pool, cmd->argv[1]);
    c->argv[1] = pcalloc(c->pool, sizeof(unsigned int));
    *((unsigned int *) c->argv[1]) = max;
  }

  return PR_HANDLED(cmd);
}
