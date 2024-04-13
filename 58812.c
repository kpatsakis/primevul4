MODRET set_createhome(cmd_rec *cmd) {
  int bool = -1, start = 2;
  mode_t mode = (mode_t) 0700, dirmode = (mode_t) 0711;
  char *skel_path = NULL;
  config_rec *c = NULL;
  uid_t cuid = 0;
  gid_t cgid = 0, hgid = -1;
  unsigned long flags = 0UL;

  if (cmd->argc-1 < 1) {
    CONF_ERROR(cmd, "wrong number of parameters");
  }

  CHECK_CONF(cmd, CONF_ROOT|CONF_VIRTUAL|CONF_GLOBAL);

  bool = get_boolean(cmd, 1);
  if (bool == -1) {
    CONF_ERROR(cmd, "expected Boolean parameter");
  }

  /* No need to process the rest if bool is FALSE. */
  if (bool == FALSE) {
    c = add_config_param(cmd->argv[0], 1, NULL);
    c->argv[0] = pcalloc(c->pool, sizeof(unsigned char));
    *((unsigned char *) c->argv[0]) = bool;

    return PR_HANDLED(cmd);
  }

  /* Check the mode parameter, if present */
  if (cmd->argc-1 >= 2 &&
      strcasecmp(cmd->argv[2], "dirmode") != 0 &&
      strcasecmp(cmd->argv[2], "skel") != 0) {
    char *tmp = NULL;

    mode = strtol(cmd->argv[2], &tmp, 8);

    if (tmp && *tmp)
      CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, ": bad mode parameter: '",
        cmd->argv[2], "'", NULL));

    start = 3;
  }

  if (cmd->argc-1 > 2) {
    register unsigned int i;

    /* Cycle through the rest of the parameters */
    for (i = start; i < cmd->argc;) {
      if (strcasecmp(cmd->argv[i], "skel") == 0) {
        struct stat st;

        /* Check that the skel directory, if configured, meets the
         * requirements.
         */

        skel_path = cmd->argv[++i];

        if (*skel_path != '/') {
          CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "skel path '",
            skel_path, "' is not a full path", NULL));
        }

        if (pr_fsio_stat(skel_path, &st) < 0) {
          CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "unable to stat '",
            skel_path, "': ", strerror(errno), NULL));
        }

        if (!S_ISDIR(st.st_mode)) {
          CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "'", skel_path,
            "' is not a directory", NULL));
        }

        /* Must not be world-writable. */
        if (st.st_mode & S_IWOTH) {
          CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "'", skel_path,
            "' is world-writable", NULL));
        }

        /* Move the index past the skel parameter */
        i++;

      } else if (strcasecmp(cmd->argv[i], "dirmode") == 0) {
        char *tmp = NULL;

        dirmode = strtol(cmd->argv[++i], &tmp, 8);
 
        if (tmp && *tmp)
          CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "bad mode parameter: '",
            cmd->argv[i], "'", NULL));

        /* Move the index past the dirmode parameter */
        i++;

      } else if (strcasecmp(cmd->argv[i], "uid") == 0) {

        /* Check for a "~" parameter. */
        if (strncmp(cmd->argv[i+1], "~", 2) != 0) {
          uid_t uid;

          if (pr_str2uid(cmd->argv[++i], &uid) < 0) { 
            CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "bad UID parameter: '",
              cmd->argv[i], "'", NULL));
          }

          cuid = uid;

        } else {
          cuid = (uid_t) -1;       
          i++;
        }

        /* Move the index past the uid parameter */
        i++;

      } else if (strcasecmp(cmd->argv[i], "gid") == 0) {

        /* Check for a "~" parameter. */
        if (strncmp(cmd->argv[i+1], "~", 2) != 0) {
          gid_t gid;

          if (pr_str2gid(cmd->argv[++i], &gid) < 0) {
            CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "bad GID parameter: '",
              cmd->argv[i], "'", NULL));
          }

          cgid = gid;

        } else {
          cgid = (gid_t) -1;
          i++;
        }

        /* Move the index past the gid parameter */
        i++;

      } else if (strcasecmp(cmd->argv[i], "homegid") == 0) {
        char *tmp = NULL;
        gid_t gid;

        gid = strtol(cmd->argv[++i], &tmp, 10);

        if (tmp && *tmp) {
          CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "bad GID parameter: '",
            cmd->argv[i], "'", NULL));
        }

        hgid = gid;

        /* Move the index past the homegid parameter */
        i++;

      } else if (strcasecmp(cmd->argv[i], "NoRootPrivs") == 0) {
        flags |= PR_MKHOME_FL_USE_USER_PRIVS;
        i++;

      } else {
        CONF_ERROR(cmd, pstrcat(cmd->tmp_pool, "unknown parameter: '",
          cmd->argv[i], "'", NULL));
      }
    }
  }

  c = add_config_param(cmd->argv[0], 8, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL);

  c->argv[0] = pcalloc(c->pool, sizeof(unsigned char));
  *((unsigned char *) c->argv[0]) = bool;
  c->argv[1] = pcalloc(c->pool, sizeof(mode_t));
  *((mode_t *) c->argv[1]) = mode;
  c->argv[2] = pcalloc(c->pool, sizeof(mode_t));
  *((mode_t *) c->argv[2]) = dirmode;

  if (skel_path) {
    c->argv[3] = pstrdup(c->pool, skel_path);
  }

  c->argv[4] = pcalloc(c->pool, sizeof(uid_t));
  *((uid_t *) c->argv[4]) = cuid;
  c->argv[5] = pcalloc(c->pool, sizeof(gid_t));
  *((gid_t *) c->argv[5]) = cgid;
  c->argv[6] = pcalloc(c->pool, sizeof(gid_t));
  *((gid_t *) c->argv[6]) = hgid;
  c->argv[7] = pcalloc(c->pool, sizeof(unsigned long));
  *((unsigned long *) c->argv[7]) = flags;
 
  return PR_HANDLED(cmd);
}
