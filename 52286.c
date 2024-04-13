skip_stream(struct archive_read *a, size_t skip_bytes)
{
	struct _7zip *zip = (struct _7zip *)a->format->data;
	const void *p;
	int64_t skipped_bytes;
	size_t bytes = skip_bytes;

	if (zip->folder_index == 0) {
		/*
		 * Optimization for a list mode.
		 * Avoid unncecessary decoding operations.
		 */
		zip->si.ci.folders[zip->entry->folderIndex].skipped_bytes
		    += skip_bytes;
		return (skip_bytes);
	}

	while (bytes) {
		skipped_bytes = read_stream(a, &p, bytes, 0);
		if (skipped_bytes < 0)
			return (skipped_bytes);
		if (skipped_bytes == 0) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Truncated 7-Zip file body");
			return (ARCHIVE_FATAL);
		}
		bytes -= (size_t)skipped_bytes;
		if (zip->pack_stream_bytes_unconsumed)
			read_consume(a);
	}
	return (skip_bytes);
}
