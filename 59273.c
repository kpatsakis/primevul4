static int perf_release(struct inode *inode, struct file *file)
{
	perf_event_release_kernel(file->private_data);
	return 0;
}
