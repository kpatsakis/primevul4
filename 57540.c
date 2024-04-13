slow_search_backward_ic(OnigEncoding enc, int case_fold_flag,
			UChar* target, UChar* target_end,
			const UChar* text, const UChar* adjust_text,
			const UChar* text_end, const UChar* text_start)
{
  UChar *s;

  s = (UChar* )text_end;
  s -= (target_end - target);
  if (s > text_start)
    s = (UChar* )text_start;
  else
    s = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, adjust_text, s);

  while (s >= text) {
    if (str_lower_case_match(enc, case_fold_flag,
                             target, target_end, s, text_end))
      return s;

    s = (UChar* )onigenc_get_prev_char_head(enc, adjust_text, s);
  }

  return (UChar* )NULL;
}
