static int apparmor_getprocattr(struct task_struct *task, char *name,
				char **value)
{
	int error = -ENOENT;
	struct aa_profile *profile;
	/* released below */
	const struct cred *cred = get_task_cred(task);
	struct aa_task_cxt *cxt = cred->security;
	profile = aa_cred_profile(cred);

	if (strcmp(name, "current") == 0)
		error = aa_getprocattr(aa_newest_version(cxt->profile),
				       value);
	else if (strcmp(name, "prev") == 0  && cxt->previous)
		error = aa_getprocattr(aa_newest_version(cxt->previous),
				       value);
	else if (strcmp(name, "exec") == 0 && cxt->onexec)
		error = aa_getprocattr(aa_newest_version(cxt->onexec),
				       value);
	else
		error = -EINVAL;

	put_cred(cred);

	return error;
}
