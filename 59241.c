struct file *perf_event_get(unsigned int fd)
{
	struct file *file;

	file = fget_raw(fd);
	if (!file)
		return ERR_PTR(-EBADF);

	if (file->f_op != &perf_fops) {
		fput(file);
		return ERR_PTR(-EBADF);
	}

	return file;
}
