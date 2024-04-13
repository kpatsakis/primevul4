static int btrfs_rename2(struct inode *old_dir, struct dentry *old_dentry,
			 struct inode *new_dir, struct dentry *new_dentry,
			 unsigned int flags)
{
	if (flags & ~RENAME_NOREPLACE)
		return -EINVAL;

	return btrfs_rename(old_dir, old_dentry, new_dir, new_dentry);
}
