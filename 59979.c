get_max_sample_threshold(void)
{
  int32_t pct =
    networkstatus_get_param(NULL, "guard-max-sample-threshold-percent",
                            DFLT_MAX_SAMPLE_THRESHOLD_PERCENT,
                            1, 100);
  return pct / 100.0;
}
