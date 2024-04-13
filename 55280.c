static int tower_open (struct inode *inode, struct file *file)
{
	struct lego_usb_tower *dev = NULL;
	int subminor;
	int retval = 0;
	struct usb_interface *interface;
	struct tower_reset_reply reset_reply;
	int result;

	nonseekable_open(inode, file);
	subminor = iminor(inode);

	interface = usb_find_interface (&tower_driver, subminor);

	if (!interface) {
		pr_err("error, can't find device for minor %d\n", subminor);
		retval = -ENODEV;
		goto exit;
	}

	mutex_lock(&open_disc_mutex);
	dev = usb_get_intfdata(interface);

	if (!dev) {
		mutex_unlock(&open_disc_mutex);
		retval = -ENODEV;
		goto exit;
	}

	/* lock this device */
	if (mutex_lock_interruptible(&dev->lock)) {
		mutex_unlock(&open_disc_mutex);
	        retval = -ERESTARTSYS;
		goto exit;
	}


	/* allow opening only once */
	if (dev->open_count) {
		mutex_unlock(&open_disc_mutex);
		retval = -EBUSY;
		goto unlock_exit;
	}
	dev->open_count = 1;
	mutex_unlock(&open_disc_mutex);

	/* reset the tower */
	result = usb_control_msg (dev->udev,
				  usb_rcvctrlpipe(dev->udev, 0),
				  LEGO_USB_TOWER_REQUEST_RESET,
				  USB_TYPE_VENDOR | USB_DIR_IN | USB_RECIP_DEVICE,
				  0,
				  0,
				  &reset_reply,
				  sizeof(reset_reply),
				  1000);
	if (result < 0) {
		dev_err(&dev->udev->dev,
			"LEGO USB Tower reset control request failed\n");
		retval = result;
		goto unlock_exit;
	}

	/* initialize in direction */
	dev->read_buffer_length = 0;
	dev->read_packet_length = 0;
	usb_fill_int_urb (dev->interrupt_in_urb,
			  dev->udev,
			  usb_rcvintpipe(dev->udev, dev->interrupt_in_endpoint->bEndpointAddress),
			  dev->interrupt_in_buffer,
			  usb_endpoint_maxp(dev->interrupt_in_endpoint),
			  tower_interrupt_in_callback,
			  dev,
			  dev->interrupt_in_interval);

	dev->interrupt_in_running = 1;
	dev->interrupt_in_done = 0;
	mb();

	retval = usb_submit_urb (dev->interrupt_in_urb, GFP_KERNEL);
	if (retval) {
		dev_err(&dev->udev->dev,
			"Couldn't submit interrupt_in_urb %d\n", retval);
		dev->interrupt_in_running = 0;
		dev->open_count = 0;
		goto unlock_exit;
	}

	/* save device in the file's private structure */
	file->private_data = dev;

unlock_exit:
	mutex_unlock(&dev->lock);

exit:
	return retval;
}
