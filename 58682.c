static void debugfs_evict_inode(struct inode *inode)
{
	truncate_inode_pages_final(&inode->i_data);
	clear_inode(inode);
	if (S_ISLNK(inode->i_mode))
		kfree(inode->i_link);
}
