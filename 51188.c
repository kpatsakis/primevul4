_archive_read_close(struct archive *_a)
{
	struct archive_read *a = (struct archive_read *)_a;
	int r = ARCHIVE_OK, r1 = ARCHIVE_OK;

	archive_check_magic(&a->archive, ARCHIVE_READ_MAGIC,
	    ARCHIVE_STATE_ANY | ARCHIVE_STATE_FATAL, "archive_read_close");
	if (a->archive.state == ARCHIVE_STATE_CLOSED)
		return (ARCHIVE_OK);
	archive_clear_error(&a->archive);
	a->archive.state = ARCHIVE_STATE_CLOSED;

	/* TODO: Clean up the formatters. */

	/* Release the filter objects. */
	r1 = __archive_read_close_filters(a);
	if (r1 < r)
		r = r1;

	return (r);
}
