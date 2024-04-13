archive_read_format_mtree_options(struct archive_read *a,
    const char *key, const char *val)
{
	struct mtree *mtree;

	mtree = (struct mtree *)(a->format->data);
	if (strcmp(key, "checkfs")  == 0) {
		/* Allows to read information missing from the mtree from the file system */
		if (val == NULL || val[0] == 0) {
			mtree->checkfs = 0;
		} else {
			mtree->checkfs = 1;
		}
		return (ARCHIVE_OK);
	}

	/* Note: The "warn" return is just to inform the options
	 * supervisor that we didn't handle it.  It will generate
	 * a suitable error if no one used this option. */
	return (ARCHIVE_WARN);
}
