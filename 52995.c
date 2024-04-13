static int iowarrior_open(struct inode *inode, struct file *file)
{
	struct iowarrior *dev = NULL;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	mutex_lock(&iowarrior_mutex);
	subminor = iminor(inode);

	interface = usb_find_interface(&iowarrior_driver, subminor);
	if (!interface) {
		mutex_unlock(&iowarrior_mutex);
		printk(KERN_ERR "%s - error, can't find device for minor %d\n",
		       __func__, subminor);
		return -ENODEV;
	}

	mutex_lock(&iowarrior_open_disc_lock);
	dev = usb_get_intfdata(interface);
	if (!dev) {
		mutex_unlock(&iowarrior_open_disc_lock);
		mutex_unlock(&iowarrior_mutex);
		return -ENODEV;
	}

	mutex_lock(&dev->mutex);
	mutex_unlock(&iowarrior_open_disc_lock);

	/* Only one process can open each device, no sharing. */
	if (dev->opened) {
		retval = -EBUSY;
		goto out;
	}

	/* setup interrupt handler for receiving values */
	if ((retval = usb_submit_urb(dev->int_in_urb, GFP_KERNEL)) < 0) {
		dev_err(&interface->dev, "Error %d while submitting URB\n", retval);
		retval = -EFAULT;
		goto out;
	}
	/* increment our usage count for the driver */
	++dev->opened;
	/* save our object in the file's private structure */
	file->private_data = dev;
	retval = 0;

out:
	mutex_unlock(&dev->mutex);
	mutex_unlock(&iowarrior_mutex);
	return retval;
}
