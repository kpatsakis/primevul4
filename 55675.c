archive_read_support_format_xar(struct archive *_a)
{
	struct xar *xar;
	struct archive_read *a = (struct archive_read *)_a;
	int r;

	archive_check_magic(_a, ARCHIVE_READ_MAGIC,
	    ARCHIVE_STATE_NEW, "archive_read_support_format_xar");

	xar = (struct xar *)calloc(1, sizeof(*xar));
	if (xar == NULL) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate xar data");
		return (ARCHIVE_FATAL);
	}

	r = __archive_read_register_format(a,
	    xar,
	    "xar",
	    xar_bid,
	    NULL,
	    xar_read_header,
	    xar_read_data,
	    xar_read_data_skip,
	    NULL,
	    xar_cleanup,
	    NULL,
	    NULL);
	if (r != ARCHIVE_OK)
		free(xar);
	return (r);
}
