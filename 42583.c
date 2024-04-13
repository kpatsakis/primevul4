static int apparmor_capable(struct task_struct *task, const struct cred *cred,
			    struct user_namespace *ns, int cap, int audit)
{
	struct aa_profile *profile;
	/* cap_capable returns 0 on success, else -EPERM */
	int error = cap_capable(task, cred, ns, cap, audit);
	if (!error) {
		profile = aa_cred_profile(cred);
		if (!unconfined(profile))
			error = aa_capable(task, profile, cap, audit);
	}
	return error;
}
