static inline int common_perm_path(int op, const struct path *path, u32 mask)
{
	struct path_cond cond = { d_backing_inode(path->dentry)->i_uid,
				  d_backing_inode(path->dentry)->i_mode
	};
	if (!mediated_filesystem(path->dentry))
		return 0;

	return common_perm(op, path, mask, &cond);
}
