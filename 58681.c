struct dentry *debugfs_create_symlink(const char *name, struct dentry *parent,
				      const char *target)
{
	struct dentry *dentry;
	struct inode *inode;
	char *link = kstrdup(target, GFP_KERNEL);
	if (!link)
		return NULL;

	dentry = start_creating(name, parent);
	if (IS_ERR(dentry)) {
		kfree(link);
		return NULL;
	}

	inode = debugfs_get_inode(dentry->d_sb);
	if (unlikely(!inode)) {
		kfree(link);
		return failed_creating(dentry);
	}
	inode->i_mode = S_IFLNK | S_IRWXUGO;
	inode->i_op = &simple_symlink_inode_operations;
	inode->i_link = link;
	d_instantiate(dentry, inode);
	return end_creating(dentry);
}
