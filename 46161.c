static struct dentry *btrfs_lookup(struct inode *dir, struct dentry *dentry,
				   unsigned int flags)
{
	struct inode *inode;

	inode = btrfs_lookup_dentry(dir, dentry);
	if (IS_ERR(inode)) {
		if (PTR_ERR(inode) == -ENOENT)
			inode = NULL;
		else
			return ERR_CAST(inode);
	}

	return d_splice_alias(inode, dentry);
}
