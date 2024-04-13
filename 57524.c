onig_match(regex_t* reg, const UChar* str, const UChar* end, const UChar* at, OnigRegion* region,
	    OnigOptionType option)
{
  int r;
  UChar *prev;
  OnigMatchArg msa;

  MATCH_ARG_INIT(msa, reg, option, region, at);
#ifdef USE_COMBINATION_EXPLOSION_CHECK
  {
    int offset = at - str;
    STATE_CHECK_BUFF_INIT(msa, end - str, offset, reg->num_comb_exp_check);
  }
#endif

  if (region
#ifdef USE_POSIX_API_REGION_OPTION
      && !IS_POSIX_REGION(option)
#endif
      ) {
    r = onig_region_resize_clear(region, reg->num_mem + 1);
  }
  else
    r = 0;

  if (r == 0) {
    if (ONIG_IS_OPTION_ON(option, ONIG_OPTION_CHECK_VALIDITY_OF_STRING)) {
      if (! ONIGENC_IS_VALID_MBC_STRING(reg->enc, str, end)) {
        r = ONIGERR_INVALID_WIDE_CHAR_VALUE;
        goto end;
      }
    }

    prev = (UChar* )onigenc_get_prev_char_head(reg->enc, str, at);
    r = match_at(reg, str, end,
#ifdef USE_MATCH_RANGE_MUST_BE_INSIDE_OF_SPECIFIED_RANGE
		 end,
#endif
		 at, prev, &msa);
  }

 end:
  MATCH_ARG_FREE(msa);
  return r;
}
