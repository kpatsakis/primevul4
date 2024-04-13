get_extreme_restriction_threshold(void)
{
  int32_t pct = networkstatus_get_param(NULL,
                                        "guard-extreme-restriction-percent",
                                        DFLT_EXTREME_RESTRICTION_PERCENT,
                                        1, INT32_MAX);
  return pct / 100.0;
}
