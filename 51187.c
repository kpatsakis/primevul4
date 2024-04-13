_archive_filter_name(struct archive *_a, int n)
{
	struct archive_read_filter *f = get_filter(_a, n);
	return f == NULL ? NULL : f->name;
}
