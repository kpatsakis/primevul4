__archive_read_free_filters(struct archive_read *a)
{
	while (a->filter != NULL) {
		struct archive_read_filter *t = a->filter->upstream;
		free(a->filter);
		a->filter = t;
	}
}
