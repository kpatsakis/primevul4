static int lock_trace(struct task_struct *task)
{
	int err = mutex_lock_killable(&task->signal->cred_guard_mutex);
	if (err)
		return err;
	if (!ptrace_may_access(task, PTRACE_MODE_ATTACH_FSCREDS)) {
		mutex_unlock(&task->signal->cred_guard_mutex);
		return -EPERM;
	}
	return 0;
}
