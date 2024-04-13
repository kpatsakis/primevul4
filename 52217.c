add_option(struct archive_read *a, struct mtree_option **global,
    const char *value, size_t len)
{
	struct mtree_option *opt;

	if ((opt = malloc(sizeof(*opt))) == NULL) {
		archive_set_error(&a->archive, errno, "Can't allocate memory");
		return (ARCHIVE_FATAL);
	}
	if ((opt->value = malloc(len + 1)) == NULL) {
		free(opt);
		archive_set_error(&a->archive, errno, "Can't allocate memory");
		return (ARCHIVE_FATAL);
	}
	memcpy(opt->value, value, len);
	opt->value[len] = '\0';
	opt->next = *global;
	*global = opt;
	return (ARCHIVE_OK);
}
