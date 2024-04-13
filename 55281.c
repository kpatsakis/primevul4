static unsigned int tower_poll (struct file *file, poll_table *wait)
{
	struct lego_usb_tower *dev;
	unsigned int mask = 0;

	dev = file->private_data;

	if (!dev->udev)
		return POLLERR | POLLHUP;

	poll_wait(file, &dev->read_wait, wait);
	poll_wait(file, &dev->write_wait, wait);

	tower_check_for_read_packet(dev);
	if (dev->read_packet_length > 0) {
		mask |= POLLIN | POLLRDNORM;
	}
	if (!dev->interrupt_out_busy) {
		mask |= POLLOUT | POLLWRNORM;
	}

	return mask;
}
