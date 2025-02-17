filter_lookup(struct archive *_a, int n)
{
	struct archive_write *a = (struct archive_write *)_a;
	struct archive_write_filter *f = a->filter_first;
	if (n == -1)
		return a->filter_last;
	if (n < 0)
		return NULL;
	while (n > 0 && f != NULL) {
		f = f->next_filter;
		--n;
	}
	return f;
}
