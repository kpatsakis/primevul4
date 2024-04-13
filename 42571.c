int brnf_sysctl_call_tables(ctl_table * ctl, int write,
			    void __user * buffer, size_t * lenp, loff_t * ppos)
{
	int ret;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);

	if (write && *(int *)(ctl->data))
		*(int *)(ctl->data) = 1;
	return ret;
}
