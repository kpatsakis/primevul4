static ssize_t comm_write(struct file *file, const char __user *buf,
				size_t count, loff_t *offset)
{
	struct inode *inode = file_inode(file);
	struct task_struct *p;
	char buffer[TASK_COMM_LEN];
	const size_t maxlen = sizeof(buffer) - 1;

	memset(buffer, 0, sizeof(buffer));
	if (copy_from_user(buffer, buf, count > maxlen ? maxlen : count))
		return -EFAULT;

	p = get_proc_task(inode);
	if (!p)
		return -ESRCH;

	if (same_thread_group(current, p))
		set_task_comm(p, buffer);
	else
		count = -EINVAL;

	put_task_struct(p);

	return count;
}
