map_search(OnigEncoding enc, UChar map[],
	   const UChar* text, const UChar* text_range)
{
  const UChar *s = text;

  while (s < text_range) {
    if (map[*s]) return (UChar* )s;

    s += enclen(enc, s);
  }
  return (UChar* )NULL;
}
