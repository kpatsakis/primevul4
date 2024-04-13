_archive_filter_name(struct archive *_a, int n)
{
	struct archive_write_filter *f = filter_lookup(_a, n);
	return f == NULL ? NULL : f->name;
}
