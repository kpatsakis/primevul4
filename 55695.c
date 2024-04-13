unknowntag_start(struct archive_read *a, struct xar *xar, const char *name)
{
	struct unknown_tag *tag;

	tag = malloc(sizeof(*tag));
	if (tag == NULL) {
		archive_set_error(&a->archive, ENOMEM, "Out of memory");
		return (ARCHIVE_FATAL);
	}
	tag->next = xar->unknowntags;
	archive_string_init(&(tag->name));
	archive_strcpy(&(tag->name), name);
	if (xar->unknowntags == NULL) {
#if DEBUG
		fprintf(stderr, "UNKNOWNTAG_START:%s\n", name);
#endif
		xar->xmlsts_unknown = xar->xmlsts;
		xar->xmlsts = UNKNOWN;
	}
	xar->unknowntags = tag;
	return (ARCHIVE_OK);
}
