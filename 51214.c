archive_seek_data(struct archive *_a, int64_t offset, int whence)
{
	struct archive_read *a = (struct archive_read *)_a;
	archive_check_magic(_a, ARCHIVE_READ_MAGIC, ARCHIVE_STATE_DATA,
	    "archive_seek_data_block");

	if (a->format->seek_data == NULL) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
		    "Internal error: "
		    "No format_seek_data_block function registered");
		return (ARCHIVE_FATAL);
	}

	return (a->format->seek_data)(a, offset, whence);
}
