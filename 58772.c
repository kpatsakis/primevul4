static void auth_exit_ev(const void *event_data, void *user_data) {
  pr_auth_cache_clear();

  /* Close the scoreboard descriptor that we opened. */
  (void) pr_close_scoreboard(FALSE);
}
