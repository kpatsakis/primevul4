zip_deflate_init(struct archive_read *a, struct zip *zip)
{
	int r;

	/* If we haven't yet read any data, initialize the decompressor. */
	if (!zip->decompress_init) {
		if (zip->stream_valid)
			r = inflateReset(&zip->stream);
		else
			r = inflateInit2(&zip->stream,
			    -15 /* Don't check for zlib header */);
		if (r != Z_OK) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Can't initialize ZIP decompression.");
			return (ARCHIVE_FATAL);
		}
		/* Stream structure has been set up. */
		zip->stream_valid = 1;
		/* We've initialized decompression for this stream. */
		zip->decompress_init = 1;
	}
	return (ARCHIVE_OK);
}
