static int environ_open(struct inode *inode, struct file *file)
{
	return __mem_open(inode, file, PTRACE_MODE_READ);
}
