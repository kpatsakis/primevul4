int d_instantiate_no_diralias(struct dentry *entry, struct inode *inode)
{
	BUG_ON(!hlist_unhashed(&entry->d_u.d_alias));

	security_d_instantiate(entry, inode);
	spin_lock(&inode->i_lock);
	if (S_ISDIR(inode->i_mode) && !hlist_empty(&inode->i_dentry)) {
		spin_unlock(&inode->i_lock);
		iput(inode);
		return -EBUSY;
	}
	__d_instantiate(entry, inode);
	spin_unlock(&inode->i_lock);

	return 0;
}
