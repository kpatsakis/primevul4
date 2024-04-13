static ssize_t lstats_write(struct file *file, const char __user *buf,
			    size_t count, loff_t *offs)
{
	struct task_struct *task = get_proc_task(file_inode(file));

	if (!task)
		return -ESRCH;
	clear_all_latency_tracing(task);
	put_task_struct(task);

	return count;
}
