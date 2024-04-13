static int ptrace_getsiginfo(struct task_struct *child, siginfo_t *info)
{
	unsigned long flags;
	int error = -ESRCH;

	if (lock_task_sighand(child, &flags)) {
		error = -EINVAL;
		if (likely(child->last_siginfo != NULL)) {
			*info = *child->last_siginfo;
			error = 0;
		}
		unlock_task_sighand(child, &flags);
	}
	return error;
}
