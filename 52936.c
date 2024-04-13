static uintmax_t parse_mark_ref_space(const char **p)
{
	uintmax_t mark;
	char *end;

	mark = parse_mark_ref(*p, &end);
	if (*end++ != ' ')
		die("Missing space after mark: %s", command_buf.buf);
	*p = end;
	return mark;
}
