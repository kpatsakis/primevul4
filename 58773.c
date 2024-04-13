static config_rec *auth_group(pool *p, const char *user, char **group,
    char **ournamep, char **anonnamep, char *pass) {
  config_rec *c;
  char *ourname = NULL, *anonname = NULL;
  char **grmem;
  struct group *grp;

  ourname = get_param_ptr(main_server->conf, "UserName", FALSE);
  if (ournamep != NULL &&
      ourname != NULL) {
    *ournamep = ourname;
  }

  c = find_config(main_server->conf, CONF_PARAM, "GroupPassword", TRUE);
  if (c) do {
    grp = pr_auth_getgrnam(p, c->argv[0]);
    if (grp == NULL) {
      continue;
    }

    for (grmem = grp->gr_mem; *grmem; grmem++) {
      if (strcmp(*grmem, user) == 0) {
        if (pr_auth_check(p, c->argv[1], user, pass) == 0) {
          break;
        }
      }
    }

    if (*grmem) {
      if (group != NULL) {
        *group = c->argv[0];
      }

      if (c->parent != NULL) {
        c = c->parent;
      }

      if (c->config_type == CONF_ANON) {
        anonname = get_param_ptr(c->subset, "UserName", FALSE);
      }

      if (anonnamep != NULL) {
        *anonnamep = anonname;
      }

      if (anonnamep != NULL &&
          !anonname &&
          ourname != NULL) {
        *anonnamep = ourname;
      }

      break;
    }

  } while((c = find_config_next(c, c->next, CONF_PARAM, "GroupPassword",
     TRUE)) != NULL);

  return c;
}
