MODRET set_anonrejectpasswords(cmd_rec *cmd) {
#ifdef PR_USE_REGEX
  config_rec *c;
  pr_regex_t *pre = NULL;
  int notmatch = FALSE, regex_flags = REG_EXTENDED|REG_NOSUB, res = 0;
  char *pattern = NULL;

  if (cmd->argc-1 < 1 ||
      cmd->argc-1 > 2) {
    CONF_ERROR(cmd, "bad number of parameters");
  }

  CHECK_CONF(cmd, CONF_ANON);

  /* Make sure that, if present, the flags parameter is correctly formatted. */
  if (cmd->argc-1 == 2) {
    int flags = 0;

    /* We need to parse the flags parameter here, to see if any flags which
     * affect the compilation of the regex (e.g. NC) are present.
     */

    flags = pr_filter_parse_flags(cmd->tmp_pool, cmd->argv[2]);
    if (flags < 0) {
      CONF_ERROR(cmd, pstrcat(cmd->tmp_pool,
        ": badly formatted flags parameter: '", cmd->argv[2], "'", NULL));
    }

    if (flags == 0) {
      CONF_ERROR(cmd, pstrcat(cmd->tmp_pool,
        ": unknown flags '", cmd->argv[2], "'", NULL));
    }

    regex_flags |= flags;
  }

  pre = pr_regexp_alloc(&auth_module);

  pattern = cmd->argv[1];
  if (*pattern == '!') {
    notmatch = TRUE;
    pattern++;
  }

  res = pr_regexp_compile(pre, pattern, regex_flags);
  if (res != 0) {
    char errstr[200] = {'\0'};

    pr_regexp_error(res, pre, errstr, 200);
    pr_regexp_free(NULL, pre);

    CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "Unable to compile regex '",
      cmd->argv[1], "': ", errstr, NULL));
  }

  c = add_config_param(cmd->argv[0], 2, pre, NULL);
  c->argv[1] = palloc(c->pool, sizeof(int));
  *((int *) c->argv[1]) = notmatch;
  return PR_HANDLED(cmd);

#else
  CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "The ", cmd->argv[0], " directive "
    "cannot be used on this system, as you do not have POSIX compliant "
    "regex support", NULL));
#endif
}
