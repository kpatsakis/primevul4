get_nonprimary_guard_connect_timeout(void)
{
  return networkstatus_get_param(NULL,
                                 "guard-nonprimary-guard-connect-timeout",
                                 DFLT_NONPRIMARY_GUARD_CONNECT_TIMEOUT,
                                 1, INT32_MAX);
}
