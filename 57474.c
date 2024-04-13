int yr_scan_verify_match(
    YR_SCAN_CONTEXT* context,
    YR_AC_MATCH* ac_match,
    uint8_t* data,
    size_t data_size,
    size_t data_base,
    size_t offset)
{
  YR_STRING* string = ac_match->string;

  #ifdef PROFILING_ENABLED
  clock_t start = clock();
  #endif

  if (data_size - offset <= 0)
    return ERROR_SUCCESS;

  if (context->flags & SCAN_FLAGS_FAST_MODE &&
      STRING_IS_SINGLE_MATCH(string) &&
      string->matches[context->tidx].head != NULL)
    return ERROR_SUCCESS;

  if (STRING_IS_FIXED_OFFSET(string) &&
      string->fixed_offset != data_base + offset)
    return ERROR_SUCCESS;

  if (STRING_IS_LITERAL(string))
  {
    FAIL_ON_ERROR(_yr_scan_verify_literal_match(
        context, ac_match, data, data_size, data_base, offset));
  }
  else
  {
    FAIL_ON_ERROR(_yr_scan_verify_re_match(
        context, ac_match, data, data_size, data_base, offset));
  }

  #ifdef PROFILING_ENABLED
  string->clock_ticks += clock() - start;
  #endif

  return ERROR_SUCCESS;
}
