static int iowarrior_release(struct inode *inode, struct file *file)
{
	struct iowarrior *dev;
	int retval = 0;

	dev = file->private_data;
	if (dev == NULL) {
		return -ENODEV;
	}

	dev_dbg(&dev->interface->dev, "minor %d\n", dev->minor);

	/* lock our device */
	mutex_lock(&dev->mutex);

	if (dev->opened <= 0) {
		retval = -ENODEV;	/* close called more than once */
		mutex_unlock(&dev->mutex);
	} else {
		dev->opened = 0;	/* we're closing now */
		retval = 0;
		if (dev->present) {
			/*
			   The device is still connected so we only shutdown
			   pending read-/write-ops.
			 */
			usb_kill_urb(dev->int_in_urb);
			wake_up_interruptible(&dev->read_wait);
			wake_up_interruptible(&dev->write_wait);
			mutex_unlock(&dev->mutex);
		} else {
			/* The device was unplugged, cleanup resources */
			mutex_unlock(&dev->mutex);
			iowarrior_delete(dev);
		}
	}
	return retval;
}
