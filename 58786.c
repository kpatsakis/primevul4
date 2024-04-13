static void auth_sess_reinit_ev(const void *event_data, void *user_data) {
  int res;

  /* A HOST command changed the main_server pointer, reinitialize ourselves. */

  pr_event_unregister(&auth_module, "core.exit", auth_exit_ev);
  pr_event_unregister(&auth_module, "core.session-reinit", auth_sess_reinit_ev);

  pr_timer_remove(PR_TIMER_LOGIN, &auth_module);

  /* Reset the CreateHome setting. */
  mkhome = FALSE;

  /* Reset any MaxPasswordSize setting. */
  (void) pr_auth_set_max_password_len(session.pool, 0);

#if defined(PR_USE_LASTLOG)
  lastlog = FALSE;
#endif /* PR_USE_LASTLOG */
  mkhome = FALSE;

  res = auth_sess_init();
  if (res < 0) {
    pr_session_disconnect(&auth_module,
      PR_SESS_DISCONNECT_SESSION_INIT_FAILED, NULL);
  }
}
