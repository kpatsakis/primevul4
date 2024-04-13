get_nonprimary_guard_idle_timeout(void)
{
  return networkstatus_get_param(NULL,
                                 "guard-nonprimary-guard-idle-timeout",
                                 DFLT_NONPRIMARY_GUARD_IDLE_TIMEOUT,
                                 1, INT32_MAX);
}
