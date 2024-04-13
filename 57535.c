onig_scan(regex_t* reg, const UChar* str, const UChar* end,
	  OnigRegion* region, OnigOptionType option,
	  int (*scan_callback)(int, int, OnigRegion*, void*),
	  void* callback_arg)
{
  int r;
  int n;
  int rs;
  const UChar* start;

  if (ONIG_IS_OPTION_ON(option, ONIG_OPTION_CHECK_VALIDITY_OF_STRING)) {
    if (! ONIGENC_IS_VALID_MBC_STRING(reg->enc, str, end))
      return ONIGERR_INVALID_WIDE_CHAR_VALUE;

    ONIG_OPTION_OFF(option, ONIG_OPTION_CHECK_VALIDITY_OF_STRING);
  }

  n = 0;
  start = str;
  while (1) {
    r = onig_search(reg, str, end, start, end, region, option);
    if (r >= 0) {
      rs = scan_callback(n, r, region, callback_arg);
      n++;
      if (rs != 0)
        return rs;

      if (region->end[0] == start - str) {
        if (start >= end) break;
        start += enclen(reg->enc, start);
      }
      else
        start = str + region->end[0];

      if (start > end)
        break;
    }
    else if (r == ONIG_MISMATCH) {
      break;
    }
    else { /* error */
      return r;
    }
  }

  return n;
}
