archive_read_support_format_zip_streamable(struct archive *_a)
{
	struct archive_read *a = (struct archive_read *)_a;
	struct zip *zip;
	int r;

	archive_check_magic(_a, ARCHIVE_READ_MAGIC,
	    ARCHIVE_STATE_NEW, "archive_read_support_format_zip");

	zip = (struct zip *)calloc(1, sizeof(*zip));
	if (zip == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate zip data");
		return (ARCHIVE_FATAL);
	}

	/* Streamable reader doesn't support mac extensions. */
	zip->process_mac_extensions = 0;

	/*
	 * Until enough data has been read, we cannot tell about
	 * any encrypted entries yet.
	 */
	zip->has_encrypted_entries = ARCHIVE_READ_FORMAT_ENCRYPTION_DONT_KNOW;
	zip->crc32func = real_crc32;

	r = __archive_read_register_format(a,
	    zip,
	    "zip",
	    archive_read_format_zip_streamable_bid,
	    archive_read_format_zip_options,
	    archive_read_format_zip_streamable_read_header,
	    archive_read_format_zip_read_data,
	    archive_read_format_zip_read_data_skip_streamable,
	    NULL,
	    archive_read_format_zip_cleanup,
	    archive_read_support_format_zip_capabilities_streamable,
	    archive_read_format_zip_has_encrypted_entries);

	if (r != ARCHIVE_OK)
		free(zip);
	return (ARCHIVE_OK);
}
