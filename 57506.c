bm_search_backward(regex_t* reg, const UChar* target, const UChar* target_end,
		   const UChar* text, const UChar* adjust_text,
		   const UChar* text_end, const UChar* text_start)
{
  const UChar *s, *t, *p;

  s = text_end - (target_end - target);
  if (text_start < s)
    s = text_start;
  else
    s = ONIGENC_LEFT_ADJUST_CHAR_HEAD(reg->enc, adjust_text, s);

  while (s >= text) {
    p = s;
    t = target;
    while (t < target_end && *p == *t) {
      p++; t++;
    }
    if (t == target_end)
      return (UChar* )s;

    s -= reg->int_map_backward[*s];
    s = ONIGENC_LEFT_ADJUST_CHAR_HEAD(reg->enc, adjust_text, s);
  }

  return (UChar* )NULL;
}
