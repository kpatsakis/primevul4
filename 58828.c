MODRET set_uselastlog(cmd_rec *cmd) {
#ifdef PR_USE_LASTLOG
  int bool;
  config_rec *c;

  CHECK_ARGS(cmd, 1);
  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  bool = get_boolean(cmd, 1);
  if (bool == -1)
    CONF_ERROR(cmd, "expected Boolean parameter");

  c = add_config_param(cmd->argv[0], 1, NULL);
  c->argv[0] = pcalloc(c->pool, sizeof(unsigned char));
  *((unsigned char *) c->argv[0]) = bool;

  return PR_HANDLED(cmd);
#else
  CONF_ERROR(cmd, "requires lastlog support (--with-lastlog)");
#endif /* PR_USE_LASTLOG */
}
