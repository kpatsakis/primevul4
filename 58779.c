MODRET auth_post_retr(cmd_rec *cmd) {
  if (auth_anon_allow_robots == TRUE) {
    return PR_DECLINED(cmd);
  }

  if (auth_anon_allow_robots_enabled == TRUE) {
    int res;

    res = pr_unregister_fs("/robots.txt");
    if (res < 0) {
      pr_log_debug(DEBUG9, "error removing 'robots' FS for '/robots.txt': %s",
        strerror(errno));
    }

    auth_anon_allow_robots_enabled = FALSE;
  }

  return PR_DECLINED(cmd);
}
