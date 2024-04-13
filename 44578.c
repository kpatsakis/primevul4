int nfs_write_inode(struct inode *inode, struct writeback_control *wbc)
{
	return nfs_commit_unstable_pages(inode, wbc);
}
