static ssize_t timerslack_ns_write(struct file *file, const char __user *buf,
					size_t count, loff_t *offset)
{
	struct inode *inode = file_inode(file);
	struct task_struct *p;
	u64 slack_ns;
	int err;

	err = kstrtoull_from_user(buf, count, 10, &slack_ns);
	if (err < 0)
		return err;

	p = get_proc_task(inode);
	if (!p)
		return -ESRCH;

	if (ptrace_may_access(p, PTRACE_MODE_ATTACH_FSCREDS)) {
		task_lock(p);
		if (slack_ns == 0)
			p->timer_slack_ns = p->default_timer_slack_ns;
		else
			p->timer_slack_ns = slack_ns;
		task_unlock(p);
	} else
		count = -EPERM;

	put_task_struct(p);

	return count;
}
