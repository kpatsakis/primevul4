backward_search_range(regex_t* reg, const UChar* str, const UChar* end,
		      UChar* s, const UChar* range, UChar* adjrange,
		      UChar** low, UChar** high)
{
  UChar *p;

  range += reg->dmin;
  p = s;

 retry:
  switch (reg->optimize) {
  case ONIG_OPTIMIZE_EXACT:
  exact_method:
    p = slow_search_backward(reg->enc, reg->exact, reg->exact_end,
			     range, adjrange, end, p);
    break;

  case ONIG_OPTIMIZE_EXACT_IC:
    p = slow_search_backward_ic(reg->enc, reg->case_fold_flag,
                                reg->exact, reg->exact_end,
                                range, adjrange, end, p);
    break;

  case ONIG_OPTIMIZE_EXACT_BM:
  case ONIG_OPTIMIZE_EXACT_BM_NOT_REV:
#ifdef USE_INT_MAP_BACKWARD
    if (IS_NULL(reg->int_map_backward)) {
      int r;

      if (s - range < BM_BACKWARD_SEARCH_LENGTH_THRESHOLD)
        goto exact_method;

      r = set_bm_backward_skip(reg->exact, reg->exact_end, reg->enc,
                               &(reg->int_map_backward));
      if (r) return r;
    }
    p = bm_search_backward(reg, reg->exact, reg->exact_end, range, adjrange,
                           end, p);
#else
    goto exact_method;
#endif
    break;

  case ONIG_OPTIMIZE_MAP:
    p = map_search_backward(reg->enc, reg->map, range, adjrange, p);
    break;
  }

  if (p) {
    if (reg->sub_anchor) {
      UChar* prev;

      switch (reg->sub_anchor) {
      case ANCHOR_BEGIN_LINE:
        if (!ON_STR_BEGIN(p)) {
          prev = onigenc_get_prev_char_head(reg->enc, str, p);
          if (!ONIGENC_IS_MBC_NEWLINE(reg->enc, prev, end)) {
            p = prev;
            goto retry;
          }
        }
        break;

      case ANCHOR_END_LINE:
        if (ON_STR_END(p)) {
#ifndef USE_NEWLINE_AT_END_OF_STRING_HAS_EMPTY_LINE
          prev = onigenc_get_prev_char_head(reg->enc, adjrange, p);
          if (IS_NULL(prev)) goto fail;
          if (ONIGENC_IS_MBC_NEWLINE(reg->enc, prev, end)) {
            p = prev;
            goto retry;
          }
#endif
        }
        else if (! ONIGENC_IS_MBC_NEWLINE(reg->enc, p, end)
#ifdef USE_CRNL_AS_LINE_TERMINATOR
                 && ! ONIGENC_IS_MBC_CRNL(reg->enc, p, end)
#endif
                 ) {
          p = onigenc_get_prev_char_head(reg->enc, adjrange, p);
          if (IS_NULL(p)) goto fail;
          goto retry;
        }
        break;
      }
    }

    /* no needs to adjust *high, *high is used as range check only */
    if (reg->dmax != ONIG_INFINITE_DISTANCE) {
      *low  = p - reg->dmax;
      *high = p - reg->dmin;
      *high = onigenc_get_right_adjust_char_head(reg->enc, adjrange, *high);
    }

#ifdef ONIG_DEBUG_SEARCH
    fprintf(stderr, "backward_search_range: low: %d, high: %d\n",
	    (int )(*low - str), (int )(*high - str));
#endif
    return 1; /* success */
  }

 fail:
#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr, "backward_search_range: fail.\n");
#endif
  return 0; /* fail */
}
