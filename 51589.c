archive_read_format_iso9660_read_data_skip(struct archive_read *a)
{
	/* Because read_next_header always does an explicit skip
	 * to the next entry, we don't need to do anything here. */
	(void)a; /* UNUSED */
	return (ARCHIVE_OK);
}
