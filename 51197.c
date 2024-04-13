archive_read_data_skip(struct archive *_a)
{
	struct archive_read *a = (struct archive_read *)_a;
	int r;
	const void *buff;
	size_t size;
	int64_t offset;

	archive_check_magic(_a, ARCHIVE_READ_MAGIC, ARCHIVE_STATE_DATA,
	    "archive_read_data_skip");

	if (a->format->read_data_skip != NULL)
		r = (a->format->read_data_skip)(a);
	else {
		while ((r = archive_read_data_block(&a->archive,
			    &buff, &size, &offset))
		    == ARCHIVE_OK)
			;
	}

	if (r == ARCHIVE_EOF)
		r = ARCHIVE_OK;

	a->archive.state = ARCHIVE_STATE_HEADER;
	return (r);
}
