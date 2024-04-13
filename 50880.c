static void *timers_start(struct seq_file *m, loff_t *pos)
{
	struct timers_private *tp = m->private;

	tp->task = get_pid_task(tp->pid, PIDTYPE_PID);
	if (!tp->task)
		return ERR_PTR(-ESRCH);

	tp->sighand = lock_task_sighand(tp->task, &tp->flags);
	if (!tp->sighand)
		return ERR_PTR(-ESRCH);

	return seq_list_start(&tp->task->signal->posix_timers, *pos);
}
