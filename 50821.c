static ssize_t mem_read(struct file *file, char __user *buf,
			size_t count, loff_t *ppos)
{
	return mem_rw(file, buf, count, ppos, 0);
}
