static int mem_open(struct inode *inode, struct file *file)
{
	int ret = __mem_open(inode, file, PTRACE_MODE_ATTACH);

	/* OK to pass negative loff_t, we can catch out-of-range */
	file->f_mode |= FMODE_UNSIGNED_OFFSET;

	return ret;
}
