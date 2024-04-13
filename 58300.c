static ssize_t write_port(struct file *file, const char __user *buf,
			  size_t count, loff_t *ppos)
{
	unsigned long i = *ppos;
	const char __user *tmp = buf;

	if (!access_ok(VERIFY_READ, buf, count))
		return -EFAULT;
	while (count-- > 0 && i < 65536) {
		char c;

		if (__get_user(c, tmp)) {
			if (tmp > buf)
				break;
			return -EFAULT;
		}
		outb(c, i);
		i++;
		tmp++;
	}
	*ppos = i;
	return tmp-buf;
}
