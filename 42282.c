archive_write_get_bytes_in_last_block(struct archive *_a)
{
	struct archive_write *a = (struct archive_write *)_a;
	archive_check_magic(&a->archive, ARCHIVE_WRITE_MAGIC,
	    ARCHIVE_STATE_ANY, "archive_write_get_bytes_in_last_block");
	return (a->bytes_in_last_block);
}
