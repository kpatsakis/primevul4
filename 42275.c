_archive_write_close(struct archive *_a)
{
	struct archive_write *a = (struct archive_write *)_a;
	int r = ARCHIVE_OK, r1 = ARCHIVE_OK;

	archive_check_magic(&a->archive, ARCHIVE_WRITE_MAGIC,
	    ARCHIVE_STATE_ANY | ARCHIVE_STATE_FATAL,
	    "archive_write_close");
	if (a->archive.state == ARCHIVE_STATE_NEW
	    || a->archive.state == ARCHIVE_STATE_CLOSED)
		return (ARCHIVE_OK); /* Okay to close() when not open. */

	archive_clear_error(&a->archive);

	/* Finish the last entry if a finish callback is specified */
	if (a->archive.state == ARCHIVE_STATE_DATA
	    && a->format_finish_entry != NULL)
		r = ((a->format_finish_entry)(a));

	/* Finish off the archive. */
	/* TODO: have format closers invoke compression close. */
	if (a->format_close != NULL) {
		r1 = (a->format_close)(a);
		if (r1 < r)
			r = r1;
	}

	/* Finish the compression and close the stream. */
	r1 = __archive_write_close_filter(a->filter_first);
	if (r1 < r)
		r = r1;

	if (a->archive.state != ARCHIVE_STATE_FATAL)
		a->archive.state = ARCHIVE_STATE_CLOSED;
	return (r);
}
