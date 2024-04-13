MODRET auth_pre_retr(cmd_rec *cmd) {
  const char *path;
  pr_fs_t *curr_fs = NULL;
  struct stat st;

  /* Only apply this for <Anonymous> logins. */
  if (session.anon_config == NULL) {
    return PR_DECLINED(cmd);
  }

  if (auth_anon_allow_robots == TRUE) {
    return PR_DECLINED(cmd);
  }

  auth_anon_allow_robots_enabled = FALSE;

  path = dir_canonical_path(cmd->tmp_pool, cmd->arg);
  if (strcasecmp(path, "/robots.txt") != 0) {
    return PR_DECLINED(cmd);
  }

  /* If a previous REST command, with a non-zero value, has been sent, then
   * do nothing.  Ugh.
   */
  if (session.restart_pos > 0) {
    pr_log_debug(DEBUG10, "'AnonAllowRobots off' in effect, but cannot "
      "support resumed download (REST %" PR_LU " previously sent by client)",
      (pr_off_t) session.restart_pos);
    return PR_DECLINED(cmd);
  }

  pr_fs_clear_cache2(path);
  if (pr_fsio_lstat(path, &st) == 0) {
    /* There's an existing REAL "robots.txt" file on disk; use that, and
     * preserve the principle of least surprise.
     */
    pr_log_debug(DEBUG10, "'AnonAllowRobots off' in effect, but have "
      "real 'robots.txt' file on disk; using that");
    return PR_DECLINED(cmd);
  }

  curr_fs = pr_get_fs(path, NULL);
  if (curr_fs != NULL) {
    pr_fs_t *robots_fs;

    robots_fs = pr_register_fs(cmd->pool, "robots", path);
    if (robots_fs == NULL) {
      pr_log_debug(DEBUG8, "'AnonAllowRobots off' in effect, but failed to "
        "register FS: %s", strerror(errno));
      return PR_DECLINED(cmd);
    }

    /* Use enough of our own custom FSIO callbacks to be able to provide
     * a fake "robots.txt" file.
     */
    robots_fs->stat = robots_fsio_stat;
    robots_fs->fstat = robots_fsio_fstat;
    robots_fs->lstat = robots_fsio_lstat;
    robots_fs->unlink = robots_fsio_unlink;
    robots_fs->open = robots_fsio_open;
    robots_fs->close = robots_fsio_close;
    robots_fs->read = robots_fsio_read;
    robots_fs->write = robots_fsio_write;
    robots_fs->access = robots_fsio_access;
    robots_fs->faccess = robots_fsio_faccess;

    /* For all other FSIO callbacks, use the underlying FS. */
    robots_fs->rename = curr_fs->rename;
    robots_fs->lseek = curr_fs->lseek;
    robots_fs->link = curr_fs->link;
    robots_fs->readlink = curr_fs->readlink;
    robots_fs->symlink = curr_fs->symlink;
    robots_fs->ftruncate = curr_fs->ftruncate;
    robots_fs->truncate = curr_fs->truncate;
    robots_fs->chmod = curr_fs->chmod;
    robots_fs->fchmod = curr_fs->fchmod;
    robots_fs->chown = curr_fs->chown;
    robots_fs->fchown = curr_fs->fchown;
    robots_fs->lchown = curr_fs->lchown;
    robots_fs->utimes = curr_fs->utimes;
    robots_fs->futimes = curr_fs->futimes;
    robots_fs->fsync = curr_fs->fsync;

    pr_fs_clear_cache2(path);
    auth_anon_allow_robots_enabled = TRUE;
  }

  return PR_DECLINED(cmd);
}
