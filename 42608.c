static int common_perm_create(int op, struct path *dir, struct dentry *dentry,
			      u32 mask, umode_t mode)
{
	struct path_cond cond = { current_fsuid(), mode };

	if (!dir->mnt || !mediated_filesystem(dir->dentry->d_inode))
		return 0;

	return common_perm_dir_dentry(op, dir, dentry, mask, &cond);
}
