get_min_filtered_sample_size(void)
{
  return networkstatus_get_param(NULL, "guard-min-filtered-sample-size",
                                 DFLT_MIN_FILTERED_SAMPLE_SIZE,
                                 1, INT32_MAX);
}
