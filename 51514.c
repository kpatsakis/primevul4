static int common_perm(int op, const struct path *path, u32 mask,
		       struct path_cond *cond)
{
	struct aa_profile *profile;
	int error = 0;

	profile = __aa_current_profile();
	if (!unconfined(profile))
		error = aa_path_perm(op, profile, path, 0, mask, cond);

	return error;
}
