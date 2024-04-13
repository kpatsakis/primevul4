archive_write_vtable(void)
{
	static struct archive_vtable av;
	static int inited = 0;

	if (!inited) {
		av.archive_close = _archive_write_close;
		av.archive_filter_bytes = _archive_filter_bytes;
		av.archive_filter_code = _archive_filter_code;
		av.archive_filter_name = _archive_filter_name;
		av.archive_filter_count = _archive_write_filter_count;
		av.archive_free = _archive_write_free;
		av.archive_write_header = _archive_write_header;
		av.archive_write_finish_entry = _archive_write_finish_entry;
		av.archive_write_data = _archive_write_data;
		inited = 1;
	}
	return (&av);
}
