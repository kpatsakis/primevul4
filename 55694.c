unknowntag_end(struct xar *xar, const char *name)
{
	struct unknown_tag *tag;

	tag = xar->unknowntags;
	if (tag == NULL || name == NULL)
		return;
	if (strcmp(tag->name.s, name) == 0) {
		xar->unknowntags = tag->next;
		archive_string_free(&(tag->name));
		free(tag);
		if (xar->unknowntags == NULL) {
#if DEBUG
			fprintf(stderr, "UNKNOWNTAG_END:%s\n", name);
#endif
			xar->xmlsts = xar->xmlsts_unknown;
		}
	}
}
