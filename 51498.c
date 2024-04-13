static int apparmor_getprocattr(struct task_struct *task, char *name,
				char **value)
{
	int error = -ENOENT;
	/* released below */
	const struct cred *cred = get_task_cred(task);
	struct aa_task_cxt *cxt = cred_cxt(cred);
	struct aa_profile *profile = NULL;

	if (strcmp(name, "current") == 0)
		profile = aa_get_newest_profile(cxt->profile);
	else if (strcmp(name, "prev") == 0  && cxt->previous)
		profile = aa_get_newest_profile(cxt->previous);
	else if (strcmp(name, "exec") == 0 && cxt->onexec)
		profile = aa_get_newest_profile(cxt->onexec);
	else
		error = -EINVAL;

	if (profile)
		error = aa_getprocattr(profile, value);

	aa_put_profile(profile);
	put_cred(cred);

	return error;
}
