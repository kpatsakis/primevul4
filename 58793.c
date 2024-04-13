static void login_failed(pool *p, const char *user) {
#ifdef HAVE_LOGINFAILED
  const char *host, *sess_ttyname;
  int res, xerrno;

  host = pr_netaddr_get_dnsstr(session.c->remote_addr);
  sess_ttyname = pr_session_get_ttyname(p);

  PRIVS_ROOT
  res = loginfailed((char *) user, (char *) host, (char *) sess_ttyname,
    AUDIT_FAIL);
  xerrno = errno;
  PRIVS_RELINQUISH

  if (res < 0) {
    pr_trace_msg("auth", 3, "AIX loginfailed() error for user '%s', "
      "host '%s', tty '%s', reason %d: %s", user, host, sess_ttyname,
      AUDIT_FAIL, strerror(errno));
  }
#endif /* HAVE_LOGINFAILED */
}
