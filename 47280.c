const char *memmem (const char *haystack, size_t haystacklen,
	      const char *needle, size_t needlelen)
{
  const char *c;
  for (c = haystack; c <= haystack + haystacklen - needlelen; c++)
    if (!memcmp (c, needle, needlelen))
      return c;
  return 0;
}
