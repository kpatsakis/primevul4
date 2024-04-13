slow_search(OnigEncoding enc, UChar* target, UChar* target_end,
	    const UChar* text, const UChar* text_end, UChar* text_range)
{
  UChar *t, *p, *s, *end;

  end = (UChar* )text_end;
  end -= target_end - target - 1;
  if (end > text_range)
    end = text_range;

  s = (UChar* )text;

  while (s < end) {
    if (*s == *target) {
      p = s + 1;
      t = target + 1;
      while (t < target_end) {
        if (*t != *p++)
          break;
        t++;
      }
      if (t == target_end)
        return s;
    }
    s += enclen(enc, s);
  }

  return (UChar* )NULL;
}
