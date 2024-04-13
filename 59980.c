get_meaningful_restriction_threshold(void)
{
  int32_t pct = networkstatus_get_param(NULL,
                                        "guard-meaningful-restriction-percent",
                                        DFLT_MEANINGFUL_RESTRICTION_PERCENT,
                                        1, INT32_MAX);
  return pct / 100.0;
}
