MODRET auth_pre_user(cmd_rec *cmd) {

  if (saw_first_user_cmd == FALSE) {
    if (pr_trace_get_level(timing_channel)) {
      unsigned long elapsed_ms;
      uint64_t finish_ms;

      pr_gettimeofday_millis(&finish_ms);
      elapsed_ms = (unsigned long) (finish_ms - session.connect_time_ms);

      pr_trace_msg(timing_channel, 4, "Time before first USER: %lu ms",
        elapsed_ms);
    }
    saw_first_user_cmd = TRUE;
  }

  if (logged_in) {
    return PR_DECLINED(cmd);
  }

  /* Close the passwd and group databases, because libc won't let us see new
   * entries to these files without this (only in PersistentPasswd mode).
   */
  pr_auth_endpwent(cmd->tmp_pool);
  pr_auth_endgrent(cmd->tmp_pool);

  /* Check for a user name that exceeds PR_TUNABLE_LOGIN_MAX. */
  if (strlen(cmd->arg) > PR_TUNABLE_LOGIN_MAX) {
    pr_log_pri(PR_LOG_NOTICE, "USER %s (Login failed): "
      "maximum USER length exceeded", cmd->arg);
    pr_response_add_err(R_501, _("Login incorrect."));

    pr_cmd_set_errno(cmd, EPERM);
    errno = EPERM;
    return PR_ERROR(cmd);
  }

  return PR_DECLINED(cmd);
}
