rsrc_basename(const char *name, size_t name_length)
{
	const char *s, *r;

	r = s = name;
	for (;;) {
		s = memchr(s, '/', name_length - (s - name));
		if (s == NULL)
			break;
		r = ++s;
	}
	return (r);
}
