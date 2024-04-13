static int do_auth(pool *p, xaset_t *conf, const char *u, char *pw) {
  char *cpw = NULL;
  config_rec *c;

  if (conf != NULL) {
    c = find_config(conf, CONF_PARAM, "UserPassword", FALSE);
    while (c != NULL) {
      pr_signals_handle();

      if (strcmp(c->argv[0], u) == 0) {
        cpw = (char *) c->argv[1];
        break;
      }

      c = find_config_next(c, c->next, CONF_PARAM, "UserPassword", FALSE);
    }
  }

  if (cpw != NULL) {
    if (pr_auth_getpwnam(p, u) == NULL) {
      int xerrno = errno;

      if (xerrno == ENOENT) {
        pr_log_pri(PR_LOG_NOTICE, "no such user '%s'", u);
      }

      errno = xerrno;
      return PR_AUTH_NOPWD;
    }

    return pr_auth_check(p, cpw, u, pw);
  }

  return pr_auth_authenticate(p, u, pw);
}
