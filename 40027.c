static int rfcomm_sock_debugfs_open(struct inode *inode, struct file *file)
{
	return single_open(file, rfcomm_sock_debugfs_show, inode->i_private);
}
