init_task_pid(struct task_struct *task, enum pid_type type, struct pid *pid)
{
	 task->pids[type].pid = pid;
}
