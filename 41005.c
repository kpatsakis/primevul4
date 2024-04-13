static int perf_release(struct inode *inode, struct file *file)
{
	put_event(file->private_data);
	return 0;
}
