struct dentry *d_find_alias(struct inode *inode)
{
	struct dentry *de = NULL;

	if (!hlist_empty(&inode->i_dentry)) {
		spin_lock(&inode->i_lock);
		de = __d_find_alias(inode);
		spin_unlock(&inode->i_lock);
	}
	return de;
}
