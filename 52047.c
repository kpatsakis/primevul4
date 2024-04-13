static unsigned int usbdev_poll(struct file *file,
				struct poll_table_struct *wait)
{
	struct usb_dev_state *ps = file->private_data;
	unsigned int mask = 0;

	poll_wait(file, &ps->wait, wait);
	if (file->f_mode & FMODE_WRITE && !list_empty(&ps->async_completed))
		mask |= POLLOUT | POLLWRNORM;
	if (!connected(ps))
		mask |= POLLERR | POLLHUP;
	return mask;
}
