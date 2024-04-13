get_internet_likely_down_interval(void)
{
  return networkstatus_get_param(NULL, "guard-internet-likely-down-interval",
                                 DFLT_INTERNET_LIKELY_DOWN_INTERVAL,
                                 1, INT32_MAX);
}
