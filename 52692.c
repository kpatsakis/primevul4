static int clie_3_5_startup(struct usb_serial *serial)
{
	struct device *dev = &serial->dev->dev;
	int result;
	u8 *data;

	data = kmalloc(1, GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	/*
	 * Note that PEG-300 series devices expect the following two calls.
	 */

	/* get the config number */
	result = usb_control_msg(serial->dev, usb_rcvctrlpipe(serial->dev, 0),
				  USB_REQ_GET_CONFIGURATION, USB_DIR_IN,
				  0, 0, data, 1, 3000);
	if (result < 0) {
		dev_err(dev, "%s: get config number failed: %d\n",
							__func__, result);
		goto out;
	}
	if (result != 1) {
		dev_err(dev, "%s: get config number bad return length: %d\n",
							__func__, result);
		result = -EIO;
		goto out;
	}

	/* get the interface number */
	result = usb_control_msg(serial->dev, usb_rcvctrlpipe(serial->dev, 0),
				  USB_REQ_GET_INTERFACE,
				  USB_DIR_IN | USB_RECIP_INTERFACE,
				  0, 0, data, 1, 3000);
	if (result < 0) {
		dev_err(dev, "%s: get interface number failed: %d\n",
							__func__, result);
		goto out;
	}
	if (result != 1) {
		dev_err(dev,
			"%s: get interface number bad return length: %d\n",
							__func__, result);
		result = -EIO;
		goto out;
	}

	result = 0;
out:
	kfree(data);

	return result;
}
