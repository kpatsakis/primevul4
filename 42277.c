_archive_write_finish_entry(struct archive *_a)
{
	struct archive_write *a = (struct archive_write *)_a;
	int ret = ARCHIVE_OK;

	archive_check_magic(&a->archive, ARCHIVE_WRITE_MAGIC,
	    ARCHIVE_STATE_HEADER | ARCHIVE_STATE_DATA,
	    "archive_write_finish_entry");
	if (a->archive.state & ARCHIVE_STATE_DATA
	    && a->format_finish_entry != NULL)
		ret = (a->format_finish_entry)(a);
	a->archive.state = ARCHIVE_STATE_HEADER;
	return (ret);
}
