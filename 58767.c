MODRET add_defaultroot(cmd_rec *cmd) {
  config_rec *c;
  char *dir;
  unsigned int argc;
  void **argv;
  array_header *acl = NULL;

  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  if (cmd->argc < 2) {
    CONF_ERROR(cmd, "syntax: DefaultRoot <directory> [<group-expression>]");
  }

  argc = cmd->argc - 2;
  argv = cmd->argv;

  dir = *++argv;

  /* dir must be / or ~. */
  if (*dir != '/' &&
      *dir != '~') {
    CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "(", dir, ") absolute pathname "
      "required", NULL));
  }

  if (strchr(dir, '*')) {
    CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "(", dir, ") wildcards not allowed "
      "in pathname", NULL));
  }

  if (*(dir + strlen(dir) - 1) != '/') {
    dir = pstrcat(cmd->tmp_pool, dir, "/", NULL);
  }

  acl = pr_expr_create(cmd->tmp_pool, &argc, (char **) argv);
  c = add_config_param(cmd->argv[0], 0);

  c->argc = argc + 1;
  c->argv = pcalloc(c->pool, (argc + 2) * sizeof(void *));
  argv = c->argv;
  *argv++ = pstrdup(c->pool, dir);

  if (argc && acl)
    while(argc--) {
      *argv++ = pstrdup(c->pool, *((char **) acl->elts));
      acl->elts = ((char **) acl->elts) + 1;
    }

  *argv = NULL;
  return PR_HANDLED(cmd);
}
