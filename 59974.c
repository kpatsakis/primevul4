get_guard_lifetime(void)
{
  if (get_options()->GuardLifetime >= 86400)
    return get_options()->GuardLifetime;
  int32_t days;
  days = networkstatus_get_param(NULL,
                                 "guard-lifetime-days",
                                 DFLT_GUARD_LIFETIME_DAYS, 1, 365*10);
  return days * 86400;
}
