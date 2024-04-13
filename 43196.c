int udf_write_inode(struct inode *inode, struct writeback_control *wbc)
{
	return udf_update_inode(inode, wbc->sync_mode == WB_SYNC_ALL);
}
