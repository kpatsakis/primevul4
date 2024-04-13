_archive_write_free(struct archive *_a)
{
	struct archive_write *a = (struct archive_write *)_a;
	int r = ARCHIVE_OK, r1;

	if (_a == NULL)
		return (ARCHIVE_OK);
	/* It is okay to call free() in state FATAL. */
	archive_check_magic(&a->archive, ARCHIVE_WRITE_MAGIC,
	    ARCHIVE_STATE_ANY | ARCHIVE_STATE_FATAL, "archive_write_free");
	if (a->archive.state != ARCHIVE_STATE_FATAL)
		r = archive_write_close(&a->archive);

	/* Release format resources. */
	if (a->format_free != NULL) {
		r1 = (a->format_free)(a);
		if (r1 < r)
			r = r1;
	}

	__archive_write_filters_free(_a);

	/* Release various dynamic buffers. */
	free((void *)(uintptr_t)(const void *)a->nulls);
	archive_string_free(&a->archive.error_string);
	a->archive.magic = 0;
	__archive_clean(&a->archive);
	free(a);
	return (r);
}
