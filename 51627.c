static int hiddev_fasync(int fd, struct file *file, int on)
{
	struct hiddev_list *list = file->private_data;

	return fasync_helper(fd, file, on, &list->fasync);
}
