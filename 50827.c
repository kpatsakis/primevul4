static ssize_t oom_score_adj_read(struct file *file, char __user *buf,
					size_t count, loff_t *ppos)
{
	struct task_struct *task = get_proc_task(file_inode(file));
	char buffer[PROC_NUMBUF];
	short oom_score_adj = OOM_SCORE_ADJ_MIN;
	unsigned long flags;
	size_t len;

	if (!task)
		return -ESRCH;
	if (lock_task_sighand(task, &flags)) {
		oom_score_adj = task->signal->oom_score_adj;
		unlock_task_sighand(task, &flags);
	}
	put_task_struct(task);
	len = snprintf(buffer, sizeof(buffer), "%hd\n", oom_score_adj);
	return simple_read_from_buffer(buf, count, ppos, buffer, len);
}
