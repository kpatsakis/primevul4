struct dentry *d_obtain_root(struct inode *inode)
{
	return __d_obtain_alias(inode, 0);
}
