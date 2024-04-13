struct dentry *debugfs_lookup(const char *name, struct dentry *parent)
{
	struct dentry *dentry;

	if (IS_ERR(parent))
		return NULL;

	if (!parent)
		parent = debugfs_mount->mnt_root;

	inode_lock(d_inode(parent));
	dentry = lookup_one_len(name, parent, strlen(name));
	inode_unlock(d_inode(parent));

	if (IS_ERR(dentry))
		return NULL;
	if (!d_really_is_positive(dentry)) {
		dput(dentry);
		return NULL;
	}
	return dentry;
}
