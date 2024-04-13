static void timers_stop(struct seq_file *m, void *v)
{
	struct timers_private *tp = m->private;

	if (tp->sighand) {
		unlock_task_sighand(tp->task, &tp->flags);
		tp->sighand = NULL;
	}

	if (tp->task) {
		put_task_struct(tp->task);
		tp->task = NULL;
	}
}
