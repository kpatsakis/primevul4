static struct dentry * __d_find_any_alias(struct inode *inode)
{
	struct dentry *alias;

	if (hlist_empty(&inode->i_dentry))
		return NULL;
	alias = hlist_entry(inode->i_dentry.first, struct dentry, d_u.d_alias);
	__dget(alias);
	return alias;
}
