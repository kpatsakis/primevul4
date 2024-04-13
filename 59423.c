lha_read_data_none(struct archive_read *a, const void **buff,
    size_t *size, int64_t *offset)
{
	struct lha *lha = (struct lha *)(a->format->data);
	ssize_t bytes_avail;

	if (lha->entry_bytes_remaining == 0) {
		*buff = NULL;
		*size = 0;
		*offset = lha->entry_offset;
		lha->end_of_entry = 1;
		return (ARCHIVE_OK);
	}
	/*
	 * Note: '1' here is a performance optimization.
	 * Recall that the decompression layer returns a count of
	 * available bytes; asking for more than that forces the
	 * decompressor to combine reads by copying data.
	 */
	*buff = __archive_read_ahead(a, 1, &bytes_avail);
	if (bytes_avail <= 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Truncated LHa file data");
		return (ARCHIVE_FATAL);
	}
	if (bytes_avail > lha->entry_bytes_remaining)
		bytes_avail = (ssize_t)lha->entry_bytes_remaining;
	lha->entry_crc_calculated =
	    lha_crc16(lha->entry_crc_calculated, *buff, bytes_avail);
	*size = bytes_avail;
	*offset = lha->entry_offset;
	lha->entry_offset += bytes_avail;
	lha->entry_bytes_remaining -= bytes_avail;
	if (lha->entry_bytes_remaining == 0)
		lha->end_of_entry = 1;
	lha->entry_unconsumed = bytes_avail;
	return (ARCHIVE_OK);
}
