archive_read_format_7zip_read_data(struct archive_read *a,
    const void **buff, size_t *size, int64_t *offset)
{
	struct _7zip *zip;
	ssize_t bytes;
	int ret = ARCHIVE_OK;

	zip = (struct _7zip *)(a->format->data);

	if (zip->has_encrypted_entries == ARCHIVE_READ_FORMAT_ENCRYPTION_DONT_KNOW) {
		zip->has_encrypted_entries = 0;
	}

	if (zip->pack_stream_bytes_unconsumed)
		read_consume(a);

	*offset = zip->entry_offset;
	*size = 0;
	*buff = NULL;
	/*
	 * If we hit end-of-entry last time, clean up and return
	 * ARCHIVE_EOF this time.
	 */
	if (zip->end_of_entry)
		return (ARCHIVE_EOF);

	bytes = read_stream(a, buff,
		(size_t)zip->entry_bytes_remaining, 0);
	if (bytes < 0)
		return ((int)bytes);
	if (bytes == 0) {
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_FILE_FORMAT,
		    "Truncated 7-Zip file body");
		return (ARCHIVE_FATAL);
	}
	zip->entry_bytes_remaining -= bytes;
	if (zip->entry_bytes_remaining == 0)
		zip->end_of_entry = 1;

	/* Update checksum */
	if ((zip->entry->flg & CRC32_IS_SET) && bytes)
		zip->entry_crc32 = crc32(zip->entry_crc32, *buff,
		    (unsigned)bytes);

	/* If we hit the end, swallow any end-of-data marker. */
	if (zip->end_of_entry) {
		/* Check computed CRC against file contents. */
		if ((zip->entry->flg & CRC32_IS_SET) &&
			zip->si.ss.digests[zip->entry->ssIndex] !=
		    zip->entry_crc32) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "7-Zip bad CRC: 0x%lx should be 0x%lx",
			    (unsigned long)zip->entry_crc32,
			    (unsigned long)zip->si.ss.digests[
			    		zip->entry->ssIndex]);
			ret = ARCHIVE_WARN;
		}
	}

	*size = bytes;
	*offset = zip->entry_offset;
	zip->entry_offset += bytes;

	return (ret);
}
