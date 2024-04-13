map_search_backward(OnigEncoding enc, UChar map[],
		    const UChar* text, const UChar* adjust_text,
		    const UChar* text_start)
{
  const UChar *s = text_start;

  while (s >= text) {
    if (map[*s]) return (UChar* )s;

    s = onigenc_get_prev_char_head(enc, adjust_text, s);
  }
  return (UChar* )NULL;
}
