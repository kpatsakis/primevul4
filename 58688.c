void debugfs_remove(struct dentry *dentry)
{
	struct dentry *parent;
	int ret;

	if (IS_ERR_OR_NULL(dentry))
		return;

	parent = dentry->d_parent;
	inode_lock(d_inode(parent));
	ret = __debugfs_remove(dentry, parent);
	inode_unlock(d_inode(parent));
	if (!ret)
		simple_release_fs(&debugfs_mount, &debugfs_mount_count);

	synchronize_srcu(&debugfs_srcu);
}
