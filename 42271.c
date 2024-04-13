__archive_write_output(struct archive_write *a, const void *buff, size_t length)
{
	return (__archive_write_filter(a->filter_first, buff, length));
}
