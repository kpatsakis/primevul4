get_filter(struct archive *_a, int n)
{
	struct archive_read *a = (struct archive_read *)_a;
	struct archive_read_filter *f = a->filter;
	/* We use n == -1 for 'the last filter', which is always the
	 * client proxy. */
	if (n == -1 && f != NULL) {
		struct archive_read_filter *last = f;
		f = f->upstream;
		while (f != NULL) {
			last = f;
			f = f->upstream;
		}
		return (last);
	}
	if (n < 0)
		return NULL;
	while (n > 0 && f != NULL) {
		f = f->upstream;
		--n;
	}
	return (f);
}
