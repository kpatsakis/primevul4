static int timerslack_ns_show(struct seq_file *m, void *v)
{
	struct inode *inode = m->private;
	struct task_struct *p;
	int err =  0;

	p = get_proc_task(inode);
	if (!p)
		return -ESRCH;

	if (ptrace_may_access(p, PTRACE_MODE_ATTACH_FSCREDS)) {
		task_lock(p);
		seq_printf(m, "%llu\n", p->timer_slack_ns);
		task_unlock(p);
	} else
		err = -EPERM;

	put_task_struct(p);

	return err;
}
