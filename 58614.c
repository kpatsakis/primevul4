struct dentry *d_obtain_alias(struct inode *inode)
{
	return __d_obtain_alias(inode, 1);
}
