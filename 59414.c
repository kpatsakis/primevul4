archive_read_format_lha_read_data_skip(struct archive_read *a)
{
	struct lha *lha;
	int64_t bytes_skipped;

	lha = (struct lha *)(a->format->data);

	if (lha->entry_unconsumed) {
		/* Consume as much as the decompressor actually used. */
		__archive_read_consume(a, lha->entry_unconsumed);
		lha->entry_unconsumed = 0;
	}

	/* if we've already read to end of data, we're done. */
	if (lha->end_of_entry_cleanup)
		return (ARCHIVE_OK);

	/*
	 * If the length is at the beginning, we can skip the
	 * compressed data much more quickly.
	 */
	bytes_skipped = __archive_read_consume(a, lha->entry_bytes_remaining);
	if (bytes_skipped < 0)
		return (ARCHIVE_FATAL);

	/* This entry is finished and done. */
	lha->end_of_entry_cleanup = lha->end_of_entry = 1;
	return (ARCHIVE_OK);
}
