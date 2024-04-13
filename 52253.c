archive_read_format_7zip_read_data_skip(struct archive_read *a)
{
	struct _7zip *zip;
	int64_t bytes_skipped;

	zip = (struct _7zip *)(a->format->data);

	if (zip->pack_stream_bytes_unconsumed)
		read_consume(a);

	/* If we've already read to end of data, we're done. */
	if (zip->end_of_entry)
		return (ARCHIVE_OK);

	/*
	 * If the length is at the beginning, we can skip the
	 * compressed data much more quickly.
	 */
	bytes_skipped = skip_stream(a, (size_t)zip->entry_bytes_remaining);
	if (bytes_skipped < 0)
		return (ARCHIVE_FATAL);
	zip->entry_bytes_remaining = 0;

	/* This entry is finished and done. */
	zip->end_of_entry = 1;
	return (ARCHIVE_OK);
}
