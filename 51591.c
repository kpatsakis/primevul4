archive_read_support_format_iso9660(struct archive *_a)
{
	struct archive_read *a = (struct archive_read *)_a;
	struct iso9660 *iso9660;
	int r;

	archive_check_magic(_a, ARCHIVE_READ_MAGIC,
	    ARCHIVE_STATE_NEW, "archive_read_support_format_iso9660");

	iso9660 = (struct iso9660 *)calloc(1, sizeof(*iso9660));
	if (iso9660 == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate iso9660 data");
		return (ARCHIVE_FATAL);
	}
	iso9660->magic = ISO9660_MAGIC;
	iso9660->cache_files.first = NULL;
	iso9660->cache_files.last = &(iso9660->cache_files.first);
	iso9660->re_files.first = NULL;
	iso9660->re_files.last = &(iso9660->re_files.first);
	/* Enable to support Joliet extensions by default.	*/
	iso9660->opt_support_joliet = 1;
	/* Enable to support Rock Ridge extensions by default.	*/
	iso9660->opt_support_rockridge = 1;

	r = __archive_read_register_format(a,
	    iso9660,
	    "iso9660",
	    archive_read_format_iso9660_bid,
	    archive_read_format_iso9660_options,
	    archive_read_format_iso9660_read_header,
	    archive_read_format_iso9660_read_data,
	    archive_read_format_iso9660_read_data_skip,
	    NULL,
	    archive_read_format_iso9660_cleanup,
	    NULL,
	    NULL);

	if (r != ARCHIVE_OK) {
		free(iso9660);
		return (r);
	}
	return (ARCHIVE_OK);
}
