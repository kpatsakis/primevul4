int pipe_proc_fn(struct ctl_table *table, int write, void __user *buf,
		 size_t *lenp, loff_t *ppos)
{
	int ret;

	ret = proc_dointvec_minmax(table, write, buf, lenp, ppos);
	if (ret < 0 || !write)
		return ret;

	pipe_max_size = round_pipe_size(pipe_max_size);
	return ret;
}
