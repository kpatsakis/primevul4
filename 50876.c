static int proc_timers_open(struct inode *inode, struct file *file)
{
	struct timers_private *tp;

	tp = __seq_open_private(file, &proc_timers_seq_ops,
			sizeof(struct timers_private));
	if (!tp)
		return -ENOMEM;

	tp->pid = proc_pid(inode);
	tp->ns = inode->i_sb->s_fs_info;
	return 0;
}
