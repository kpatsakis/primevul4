zip_get_local_file_header_size(struct archive_read *a, size_t extra)
{
	const char *p;
	ssize_t filename_length, extra_length;

	if ((p = __archive_read_ahead(a, extra + 30, NULL)) == NULL) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Truncated ZIP file header");
		return (ARCHIVE_WARN);
	}
	p += extra;

	if (memcmp(p, "PK\003\004", 4) != 0) {
		archive_set_error(&a->archive, -1, "Damaged Zip archive");
		return ARCHIVE_WARN;
	}
	filename_length = archive_le16dec(p + 26);
	extra_length = archive_le16dec(p + 28);

	return (30 + filename_length + extra_length);
}
