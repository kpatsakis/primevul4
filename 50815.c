static bool has_pid_permissions(struct pid_namespace *pid,
				 struct task_struct *task,
				 int hide_pid_min)
{
	if (pid->hide_pid < hide_pid_min)
		return true;
	if (in_group_p(pid->pid_gid))
		return true;
	return ptrace_may_access(task, PTRACE_MODE_READ_FSCREDS);
}
