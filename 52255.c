archive_read_support_format_7zip(struct archive *_a)
{
	struct archive_read *a = (struct archive_read *)_a;
	struct _7zip *zip;
	int r;

	archive_check_magic(_a, ARCHIVE_READ_MAGIC,
	    ARCHIVE_STATE_NEW, "archive_read_support_format_7zip");

	zip = calloc(1, sizeof(*zip));
	if (zip == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate 7zip data");
		return (ARCHIVE_FATAL);
	}

	/*
	 * Until enough data has been read, we cannot tell about
	 * any encrypted entries yet.
	 */
	zip->has_encrypted_entries = ARCHIVE_READ_FORMAT_ENCRYPTION_DONT_KNOW;


	r = __archive_read_register_format(a,
	    zip,
	    "7zip",
	    archive_read_format_7zip_bid,
	    NULL,
	    archive_read_format_7zip_read_header,
	    archive_read_format_7zip_read_data,
	    archive_read_format_7zip_read_data_skip,
	    NULL,
	    archive_read_format_7zip_cleanup,
	    archive_read_support_format_7zip_capabilities,
	    archive_read_format_7zip_has_encrypted_entries);

	if (r != ARCHIVE_OK)
		free(zip);
	return (ARCHIVE_OK);
}
