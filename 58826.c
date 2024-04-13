MODRET set_timeoutsession(cmd_rec *cmd) {
  int timeout = 0, precedence = 0;
  config_rec *c = NULL;

  int ctxt = (cmd->config && cmd->config->config_type != CONF_PARAM ?
     cmd->config->config_type : cmd->server->config_type ?
     cmd->server->config_type : CONF_ROOT);

  /* this directive must have either 1 or 3 arguments */
  if (cmd->argc-1 != 1 &&
      cmd->argc-1 != 3) {
    CONF_ERROR(cmd, "missing parameters");
  }

  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL|CONF_ANON);

  /* Set the precedence for this config_rec based on its configuration
   * context.
   */
  if (ctxt & CONF_GLOBAL) {
    precedence = 1;

  /* These will never appear simultaneously */
  } else if ((ctxt & CONF_ROOT) ||
             (ctxt & CONF_VIRTUAL)) {
    precedence = 2;

  } else if (ctxt & CONF_ANON) {
    precedence = 3;
  }

  if (pr_str_get_duration(cmd->argv[1], &timeout) < 0) {
    CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "error parsing timeout value '",
      cmd->argv[1], "': ", strerror(errno), NULL));
  }

  if (timeout == 0) {
    /* do nothing */
    return PR_HANDLED(cmd);
  }

  if (cmd->argc-1 == 3) {
    if (strncmp(cmd->argv[2], "user", 5) == 0 ||
        strncmp(cmd->argv[2], "group", 6) == 0 ||
        strncmp(cmd->argv[2], "class", 6) == 0) {

       /* no op */

     } else {
       CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, cmd->argv[0],
         ": unknown classifier used: '", cmd->argv[2], "'", NULL));
    }
  }

  if (cmd->argc-1 == 1) {
    c = add_config_param(cmd->argv[0], 2, NULL);
    c->argv[0] = pcalloc(c->pool, sizeof(int));
    *((int *) c->argv[0]) = timeout;
    c->argv[1] = pcalloc(c->pool, sizeof(unsigned int));
    *((unsigned int *) c->argv[1]) = precedence;

  } else if (cmd->argc-1 == 3) {
    array_header *acl = NULL;
    unsigned int argc;
    void **argv;

    argc = cmd->argc - 3;
    argv = cmd->argv + 2;

    acl = pr_expr_create(cmd->tmp_pool, &argc, (char **) argv);

    c = add_config_param(cmd->argv[0], 0);
    c->argc = argc + 2;

    /* Add 3 to argc for the argv of the config_rec: one for the
     * seconds value, one for the precedence, one for the classifier,
     * and one for the terminating NULL.
     */
    c->argv = pcalloc(c->pool, ((argc + 4) * sizeof(void *)));

    /* Capture the config_rec's argv pointer for doing the by-hand
     * population.
     */
    argv = c->argv;

    /* Copy in the seconds. */
    *argv = pcalloc(c->pool, sizeof(int));
    *((int *) *argv++) = timeout;

    /* Copy in the precedence. */
    *argv = pcalloc(c->pool, sizeof(unsigned int));
    *((unsigned int *) *argv++) = precedence;

    /* Copy in the classifier. */
    *argv++ = pstrdup(c->pool, cmd->argv[2]);

    /* now, copy in the expression arguments */
    if (argc && acl) {
      while (argc--) {
        *argv++ = pstrdup(c->pool, *((char **) acl->elts));
        acl->elts = ((char **) acl->elts) + 1;
      }
    }

    /* don't forget the terminating NULL */
    *argv = NULL;

  } else {
    /* Should never reach here. */
    CONF_ERROR(cmd, "wrong number of parameters");
  }

  c->flags |= CF_MERGEDOWN_MULTI;
  return PR_HANDLED(cmd);
}
