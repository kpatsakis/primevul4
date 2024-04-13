static int this_tty(const void *t, struct file *file, unsigned fd)
{
	if (likely(file->f_op->read != tty_read))
		return 0;
	return file_tty(file) != t ? 0 : fd + 1;
}
