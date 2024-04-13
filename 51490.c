static int apparmor_capable(const struct cred *cred, struct user_namespace *ns,
			    int cap, int audit)
{
	struct aa_profile *profile;
	int error = 0;

	profile = aa_cred_profile(cred);
	if (!unconfined(profile))
		error = aa_capable(profile, cap, audit);
	return error;
}
