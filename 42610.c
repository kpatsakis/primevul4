static int common_perm_mnt_dentry(int op, struct vfsmount *mnt,
				  struct dentry *dentry, u32 mask)
{
	struct path path = { mnt, dentry };
	struct path_cond cond = { dentry->d_inode->i_uid,
				  dentry->d_inode->i_mode
	};

	return common_perm(op, &path, mask, &cond);
}
