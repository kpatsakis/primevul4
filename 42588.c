static int apparmor_dentry_open(struct file *file, const struct cred *cred)
{
	struct aa_file_cxt *fcxt = file->f_security;
	struct aa_profile *profile;
	int error = 0;

	if (!mediated_filesystem(file->f_path.dentry->d_inode))
		return 0;

	/* If in exec, permission is handled by bprm hooks.
	 * Cache permissions granted by the previous exec check, with
	 * implicit read and executable mmap which are required to
	 * actually execute the image.
	 */
	if (current->in_execve) {
		fcxt->allow = MAY_EXEC | MAY_READ | AA_EXEC_MMAP;
		return 0;
	}

	profile = aa_cred_profile(cred);
	if (!unconfined(profile)) {
		struct inode *inode = file->f_path.dentry->d_inode;
		struct path_cond cond = { inode->i_uid, inode->i_mode };

		error = aa_path_perm(OP_OPEN, profile, &file->f_path, 0,
				     aa_map_file_to_perms(file), &cond);
		/* todo cache full allowed permissions set and state */
		fcxt->allow = aa_map_file_to_perms(file);
	}

	return error;
}
