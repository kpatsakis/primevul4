static ssize_t oom_adj_read(struct file *file, char __user *buf, size_t count,
			    loff_t *ppos)
{
	struct task_struct *task = get_proc_task(file_inode(file));
	char buffer[PROC_NUMBUF];
	int oom_adj = OOM_ADJUST_MIN;
	size_t len;
	unsigned long flags;

	if (!task)
		return -ESRCH;
	if (lock_task_sighand(task, &flags)) {
		if (task->signal->oom_score_adj == OOM_SCORE_ADJ_MAX)
			oom_adj = OOM_ADJUST_MAX;
		else
			oom_adj = (task->signal->oom_score_adj * -OOM_DISABLE) /
				  OOM_SCORE_ADJ_MAX;
		unlock_task_sighand(task, &flags);
	}
	put_task_struct(task);
	len = snprintf(buffer, sizeof(buffer), "%d\n", oom_adj);
	return simple_read_from_buffer(buf, count, ppos, buffer, len);
}
