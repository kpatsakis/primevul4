process_global_set(struct archive_read *a,
    struct mtree_option **global, const char *line)
{
	const char *next, *eq;
	size_t len;
	int r;

	line += 4;
	for (;;) {
		next = line + strspn(line, " \t\r\n");
		if (*next == '\0')
			return (ARCHIVE_OK);
		line = next;
		next = line + strcspn(line, " \t\r\n");
		eq = strchr(line, '=');
		if (eq > next)
			len = next - line;
		else
			len = eq - line;

		remove_option(global, line, len);
		r = add_option(a, global, line, next - line);
		if (r != ARCHIVE_OK)
			return (r);
		line = next;
	}
}
