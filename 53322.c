static ssize_t hung_up_tty_write(struct file *file, const char __user *buf,
				 size_t count, loff_t *ppos)
{
	return -EIO;
}
