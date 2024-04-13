_archive_filter_code(struct archive *_a, int n)
{
	struct archive_read_filter *f = get_filter(_a, n);
	return f == NULL ? -1 : f->code;
}
