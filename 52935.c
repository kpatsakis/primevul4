static uintmax_t parse_mark_ref(const char *p, char **endptr)
{
	uintmax_t mark;

	assert(*p == ':');
	p++;
	mark = strtoumax(p, endptr, 10);
	if (*endptr == p)
		die("No value after ':' in mark: %s", command_buf.buf);
	return mark;
}
