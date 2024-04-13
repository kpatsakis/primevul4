parse_line(struct archive_read *a, struct archive_entry *entry,
    struct mtree *mtree, struct mtree_entry *mp, int *parsed_kws)
{
	struct mtree_option *iter;
	int r = ARCHIVE_OK, r1;

	for (iter = mp->options; iter != NULL; iter = iter->next) {
		r1 = parse_keyword(a, mtree, entry, iter, parsed_kws);
		if (r1 < r)
			r = r1;
	}
	if (r == ARCHIVE_OK && (*parsed_kws & MTREE_HAS_TYPE) == 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Missing type keyword in mtree specification");
		return (ARCHIVE_WARN);
	}
	return (r);
}
