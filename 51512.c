static int common_file_perm(int op, struct file *file, u32 mask)
{
	struct aa_file_cxt *fcxt = file->f_security;
	struct aa_profile *profile, *fprofile = aa_cred_profile(file->f_cred);
	int error = 0;

	BUG_ON(!fprofile);

	if (!file->f_path.mnt ||
	    !mediated_filesystem(file->f_path.dentry))
		return 0;

	profile = __aa_current_profile();

	/* revalidate access, if task is unconfined, or the cached cred
	 * doesn't match or if the request is for more permissions than
	 * was granted.
	 *
	 * Note: the test for !unconfined(fprofile) is to handle file
	 *       delegation from unconfined tasks
	 */
	if (!unconfined(profile) && !unconfined(fprofile) &&
	    ((fprofile != profile) || (mask & ~fcxt->allow)))
		error = aa_file_perm(op, profile, file, mask);

	return error;
}
