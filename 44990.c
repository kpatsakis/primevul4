static int check_mem_permission(struct task_struct *task)
{
	/*
	 * A task can always look at itself, in case it chooses
	 * to use system calls instead of load instructions.
	 */
	if (task == current)
		return 0;

	/*
	 * If current is actively ptrace'ing, and would also be
	 * permitted to freshly attach with ptrace now, permit it.
	 */
	if (task_is_stopped_or_traced(task)) {
		int match;
		rcu_read_lock();
		match = (tracehook_tracer_task(task) == current);
		rcu_read_unlock();
		if (match && ptrace_may_access(task, PTRACE_MODE_ATTACH))
			return 0;
	}

	/*
	 * Noone else is allowed.
	 */
	return -EPERM;
}
