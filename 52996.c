static unsigned iowarrior_poll(struct file *file, poll_table * wait)
{
	struct iowarrior *dev = file->private_data;
	unsigned int mask = 0;

	if (!dev->present)
		return POLLERR | POLLHUP;

	poll_wait(file, &dev->read_wait, wait);
	poll_wait(file, &dev->write_wait, wait);

	if (!dev->present)
		return POLLERR | POLLHUP;

	if (read_index(dev) != -1)
		mask |= POLLIN | POLLRDNORM;

	if (atomic_read(&dev->write_busy) < MAX_WRITES_IN_FLIGHT)
		mask |= POLLOUT | POLLWRNORM;
	return mask;
}
