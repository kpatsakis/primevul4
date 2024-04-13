get_max_sample_size_absolute(void)
{
  return (int) networkstatus_get_param(NULL, "guard-max-sample-size",
                                       DFLT_MAX_SAMPLE_SIZE,
                                       1, INT32_MAX);
}
