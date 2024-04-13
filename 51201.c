archive_read_header_position(struct archive *_a)
{
	struct archive_read *a = (struct archive_read *)_a;
	archive_check_magic(_a, ARCHIVE_READ_MAGIC,
	    ARCHIVE_STATE_ANY, "archive_read_header_position");
	return (a->header_position);
}
