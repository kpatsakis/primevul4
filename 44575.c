int nfs_wb_page(struct inode *inode, struct page *page)
{
	loff_t range_start = page_file_offset(page);
	loff_t range_end = range_start + (loff_t)(PAGE_CACHE_SIZE - 1);
	struct writeback_control wbc = {
		.sync_mode = WB_SYNC_ALL,
		.nr_to_write = 0,
		.range_start = range_start,
		.range_end = range_end,
	};
	int ret;

	trace_nfs_writeback_page_enter(inode);

	for (;;) {
		wait_on_page_writeback(page);
		if (clear_page_dirty_for_io(page)) {
			ret = nfs_writepage_locked(page, &wbc);
			if (ret < 0)
				goto out_error;
			continue;
		}
		ret = 0;
		if (!PagePrivate(page))
			break;
		ret = nfs_commit_inode(inode, FLUSH_SYNC);
		if (ret < 0)
			goto out_error;
	}
out_error:
	trace_nfs_writeback_page_exit(inode, ret);
	return ret;
}
