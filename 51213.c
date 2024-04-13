archive_read_vtable(void)
{
	static struct archive_vtable av;
	static int inited = 0;

	if (!inited) {
		av.archive_filter_bytes = _archive_filter_bytes;
		av.archive_filter_code = _archive_filter_code;
		av.archive_filter_name = _archive_filter_name;
		av.archive_filter_count = _archive_filter_count;
		av.archive_read_data_block = _archive_read_data_block;
		av.archive_read_next_header = _archive_read_next_header;
		av.archive_read_next_header2 = _archive_read_next_header2;
		av.archive_free = _archive_read_free;
		av.archive_close = _archive_read_close;
		inited = 1;
	}
	return (&av);
}
