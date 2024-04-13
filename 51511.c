 static int apparmor_task_setrlimit(struct task_struct *task,
		unsigned int resource, struct rlimit *new_rlim)
{
	struct aa_profile *profile = __aa_current_profile();
	int error = 0;

	if (!unconfined(profile))
		error = aa_task_setrlimit(profile, task, resource, new_rlim);

	return error;
}
