_archive_filter_bytes(struct archive *_a, int n)
{
	struct archive_write_filter *f = filter_lookup(_a, n);
	return f == NULL ? -1 : f->bytes_written;
}
