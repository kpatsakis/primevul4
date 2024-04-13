static int apparmor_path_link(struct dentry *old_dentry, const struct path *new_dir,
			      struct dentry *new_dentry)
{
	struct aa_profile *profile;
	int error = 0;

	if (!mediated_filesystem(old_dentry))
		return 0;

	profile = aa_current_profile();
	if (!unconfined(profile))
		error = aa_path_link(profile, old_dentry, new_dir, new_dentry);
	return error;
}
