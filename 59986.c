get_remove_unlisted_guards_after_days(void)
{
  return networkstatus_get_param(NULL,
                                 "guard-remove-unlisted-guards-after-days",
                                 DFLT_REMOVE_UNLISTED_GUARDS_AFTER_DAYS,
                                 1, 365*10);
}
