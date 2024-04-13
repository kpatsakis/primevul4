rd_contents(struct archive_read *a, const void **buff, size_t *size,
    size_t *used, uint64_t remaining)
{
	const unsigned char *b;
	ssize_t bytes;

	/* Get whatever bytes are immediately available. */
	b = __archive_read_ahead(a, 1, &bytes);
	if (bytes < 0)
		return ((int)bytes);
	if (bytes == 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Truncated archive file");
		return (ARCHIVE_FATAL);
	}
	if ((uint64_t)bytes > remaining)
		bytes = (ssize_t)remaining;

	/*
	 * Decompress contents of file.
	 */
	*used = bytes;
	if (decompress(a, buff, size, b, used) != ARCHIVE_OK)
		return (ARCHIVE_FATAL);

	/*
	 * Update checksum of a compressed data and a extracted data.
	 */
	checksum_update(a, b, *used, *buff, *size);

	return (ARCHIVE_OK);
}
