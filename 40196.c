ssize_t read_code(struct file *file, unsigned long addr, loff_t pos, size_t len)
{
	ssize_t res = file->f_op->read(file, (void __user *)addr, len, &pos);
	if (res > 0)
		flush_icache_range(addr, addr + len);
	return res;
}
