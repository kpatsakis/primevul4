static unsigned int hiddev_poll(struct file *file, poll_table *wait)
{
	struct hiddev_list *list = file->private_data;

	poll_wait(file, &list->hiddev->wait, wait);
	if (list->head != list->tail)
		return POLLIN | POLLRDNORM;
	if (!list->hiddev->exist)
		return POLLERR | POLLHUP;
	return 0;
}
