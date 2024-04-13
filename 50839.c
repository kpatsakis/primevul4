static ssize_t proc_fault_inject_write(struct file * file,
			const char __user * buf, size_t count, loff_t *ppos)
{
	struct task_struct *task;
	char buffer[PROC_NUMBUF];
	int make_it_fail;
	int rv;

	if (!capable(CAP_SYS_RESOURCE))
		return -EPERM;
	memset(buffer, 0, sizeof(buffer));
	if (count > sizeof(buffer) - 1)
		count = sizeof(buffer) - 1;
	if (copy_from_user(buffer, buf, count))
		return -EFAULT;
	rv = kstrtoint(strstrip(buffer), 0, &make_it_fail);
	if (rv < 0)
		return rv;
	if (make_it_fail < 0 || make_it_fail > 1)
		return -EINVAL;

	task = get_proc_task(file_inode(file));
	if (!task)
		return -ESRCH;
	task->make_it_fail = make_it_fail;
	put_task_struct(task);

	return count;
}
