onig_number_of_capture_histories(regex_t* reg)
{
#ifdef USE_CAPTURE_HISTORY
  int i, n;

  n = 0;
  for (i = 0; i <= ONIG_MAX_CAPTURE_HISTORY_GROUP; i++) {
    if (BIT_STATUS_AT(reg->capture_history, i) != 0)
      n++;
  }
  return n;
#else
  return 0;
#endif
}
