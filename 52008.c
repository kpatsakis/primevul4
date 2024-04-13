static int proc_bulk(struct usb_dev_state *ps, void __user *arg)
{
	struct usb_device *dev = ps->dev;
	struct usbdevfs_bulktransfer bulk;
	unsigned int tmo, len1, pipe;
	int len2;
	unsigned char *tbuf;
	int i, ret;

	if (copy_from_user(&bulk, arg, sizeof(bulk)))
		return -EFAULT;
	ret = findintfep(ps->dev, bulk.ep);
	if (ret < 0)
		return ret;
	ret = checkintf(ps, ret);
	if (ret)
		return ret;
	if (bulk.ep & USB_DIR_IN)
		pipe = usb_rcvbulkpipe(dev, bulk.ep & 0x7f);
	else
		pipe = usb_sndbulkpipe(dev, bulk.ep & 0x7f);
	if (!usb_maxpacket(dev, pipe, !(bulk.ep & USB_DIR_IN)))
		return -EINVAL;
	len1 = bulk.len;
	if (len1 >= USBFS_XFER_MAX)
		return -EINVAL;
	ret = usbfs_increase_memory_usage(len1 + sizeof(struct urb));
	if (ret)
		return ret;
	tbuf = kmalloc(len1, GFP_KERNEL);
	if (!tbuf) {
		ret = -ENOMEM;
		goto done;
	}
	tmo = bulk.timeout;
	if (bulk.ep & 0x80) {
		if (len1 && !access_ok(VERIFY_WRITE, bulk.data, len1)) {
			ret = -EINVAL;
			goto done;
		}
		snoop_urb(dev, NULL, pipe, len1, tmo, SUBMIT, NULL, 0);

		usb_unlock_device(dev);
		i = usb_bulk_msg(dev, pipe, tbuf, len1, &len2, tmo);
		usb_lock_device(dev);
		snoop_urb(dev, NULL, pipe, len2, i, COMPLETE, tbuf, len2);

		if (!i && len2) {
			if (copy_to_user(bulk.data, tbuf, len2)) {
				ret = -EFAULT;
				goto done;
			}
		}
	} else {
		if (len1) {
			if (copy_from_user(tbuf, bulk.data, len1)) {
				ret = -EFAULT;
				goto done;
			}
		}
		snoop_urb(dev, NULL, pipe, len1, tmo, SUBMIT, tbuf, len1);

		usb_unlock_device(dev);
		i = usb_bulk_msg(dev, pipe, tbuf, len1, &len2, tmo);
		usb_lock_device(dev);
		snoop_urb(dev, NULL, pipe, len2, i, COMPLETE, NULL, 0);
	}
	ret = (i < 0 ? i : len2);
 done:
	kfree(tbuf);
	usbfs_decrease_memory_usage(len1 + sizeof(struct urb));
	return ret;
}
