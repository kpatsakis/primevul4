static int auth_session_timeout_cb(CALLBACK_FRAME) {
  pr_event_generate("core.timeout-session", NULL);
  pr_response_send_async(R_421,
    _("Session Timeout (%d seconds): closing control connection"),
    TimeoutSession);

  pr_log_pri(PR_LOG_INFO, "%s", "FTP session timed out, disconnected");
  pr_session_disconnect(&auth_module, PR_SESS_DISCONNECT_TIMEOUT,
    "TimeoutSession");

  /* no need to restart the timer -- session's over */
  return 0;
}
