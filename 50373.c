static ssize_t proc_sys_read(struct file *filp, char __user *buf,
				size_t count, loff_t *ppos)
{
	return proc_sys_call_handler(filp, (void __user *)buf, count, ppos, 0);
}
