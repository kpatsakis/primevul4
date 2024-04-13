static int apparmor_path_chown(struct path *path, uid_t uid, gid_t gid)
{
	struct path_cond cond =  { path->dentry->d_inode->i_uid,
				   path->dentry->d_inode->i_mode
	};

	if (!mediated_filesystem(path->dentry->d_inode))
		return 0;

	return common_perm(OP_CHOWN, path, AA_MAY_CHOWN, &cond);
}
