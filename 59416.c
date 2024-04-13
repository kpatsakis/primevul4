archive_read_support_format_lha(struct archive *_a)
{
	struct archive_read *a = (struct archive_read *)_a;
	struct lha *lha;
	int r;

	archive_check_magic(_a, ARCHIVE_READ_MAGIC,
	    ARCHIVE_STATE_NEW, "archive_read_support_format_lha");

	lha = (struct lha *)calloc(1, sizeof(*lha));
	if (lha == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate lha data");
		return (ARCHIVE_FATAL);
	}
	archive_string_init(&lha->ws);

	r = __archive_read_register_format(a,
	    lha,
	    "lha",
	    archive_read_format_lha_bid,
	    archive_read_format_lha_options,
	    archive_read_format_lha_read_header,
	    archive_read_format_lha_read_data,
	    archive_read_format_lha_read_data_skip,
	    NULL,
	    archive_read_format_lha_cleanup,
	    NULL,
	    NULL);

	if (r != ARCHIVE_OK)
		free(lha);
	return (ARCHIVE_OK);
}
