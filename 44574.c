int nfs_wb_all(struct inode *inode)
{
	struct writeback_control wbc = {
		.sync_mode = WB_SYNC_ALL,
		.nr_to_write = LONG_MAX,
		.range_start = 0,
		.range_end = LLONG_MAX,
	};
	int ret;

	trace_nfs_writeback_inode_enter(inode);

	ret = sync_inode(inode, &wbc);

	trace_nfs_writeback_inode_exit(inode, ret);
	return ret;
}
