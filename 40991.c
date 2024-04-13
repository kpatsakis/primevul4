static int perf_fasync(int fd, struct file *filp, int on)
{
	struct inode *inode = file_inode(filp);
	struct perf_event *event = filp->private_data;
	int retval;

	mutex_lock(&inode->i_mutex);
	retval = fasync_helper(fd, filp, on, &event->fasync);
	mutex_unlock(&inode->i_mutex);

	if (retval < 0)
		return retval;

	return 0;
}
