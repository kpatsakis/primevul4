__archive_read_consume(struct archive_read *a, int64_t request)
{
	return (__archive_read_filter_consume(a->filter, request));
}
