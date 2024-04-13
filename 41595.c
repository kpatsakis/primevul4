static int wdm_open(struct inode *inode, struct file *file)
{
	int minor = iminor(inode);
	int rv = -ENODEV;
	struct usb_interface *intf;
	struct wdm_device *desc;

	mutex_lock(&wdm_mutex);
	desc = wdm_find_device_by_minor(minor);
	if (!desc)
		goto out;

	intf = desc->intf;
	if (test_bit(WDM_DISCONNECTING, &desc->flags))
		goto out;
	file->private_data = desc;

	rv = usb_autopm_get_interface(desc->intf);
	if (rv < 0) {
		dev_err(&desc->intf->dev, "Error autopm - %d\n", rv);
		goto out;
	}

	/* using write lock to protect desc->count */
	mutex_lock(&desc->wlock);
	if (!desc->count++) {
		desc->werr = 0;
		desc->rerr = 0;
		rv = usb_submit_urb(desc->validity, GFP_KERNEL);
		if (rv < 0) {
			desc->count--;
			dev_err(&desc->intf->dev,
				"Error submitting int urb - %d\n", rv);
			rv = usb_translate_errors(rv);
		}
	} else {
		rv = 0;
	}
	mutex_unlock(&desc->wlock);
	if (desc->count == 1)
		desc->manage_power(intf, 1);
	usb_autopm_put_interface(desc->intf);
out:
	mutex_unlock(&wdm_mutex);
	return rv;
}
