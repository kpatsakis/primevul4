xfs_buf_delwri_submit_nowait(
	struct list_head	*buffer_list)
{
	LIST_HEAD		(io_list);
	return __xfs_buf_delwri_submit(buffer_list, &io_list, false);
}
