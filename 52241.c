process_global_unset(struct archive_read *a,
    struct mtree_option **global, const char *line)
{
	const char *next;
	size_t len;

	line += 6;
	if (strchr(line, '=') != NULL) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "/unset shall not contain `='");
		return ARCHIVE_FATAL;
	}

	for (;;) {
		next = line + strspn(line, " \t\r\n");
		if (*next == '\0')
			return (ARCHIVE_OK);
		line = next;
		len = strcspn(line, " \t\r\n");

		if (len == 3 && strncmp(line, "all", 3) == 0) {
			free_options(*global);
			*global = NULL;
		} else {
			remove_option(global, line, len);
		}

		line += len;
	}
}
