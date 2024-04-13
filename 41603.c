static ssize_t wdm_write
(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	u8 *buf;
	int rv = -EMSGSIZE, r, we;
	struct wdm_device *desc = file->private_data;
	struct usb_ctrlrequest *req;

	if (count > desc->wMaxCommand)
		count = desc->wMaxCommand;

	spin_lock_irq(&desc->iuspin);
	we = desc->werr;
	desc->werr = 0;
	spin_unlock_irq(&desc->iuspin);
	if (we < 0)
		return -EIO;

	buf = kmalloc(count, GFP_KERNEL);
	if (!buf) {
		rv = -ENOMEM;
		goto outnl;
	}

	r = copy_from_user(buf, buffer, count);
	if (r > 0) {
		kfree(buf);
		rv = -EFAULT;
		goto outnl;
	}

	/* concurrent writes and disconnect */
	r = mutex_lock_interruptible(&desc->wlock);
	rv = -ERESTARTSYS;
	if (r) {
		kfree(buf);
		goto outnl;
	}

	if (test_bit(WDM_DISCONNECTING, &desc->flags)) {
		kfree(buf);
		rv = -ENODEV;
		goto outnp;
	}

	r = usb_autopm_get_interface(desc->intf);
	if (r < 0) {
		kfree(buf);
		rv = usb_translate_errors(r);
		goto outnp;
	}

	if (!(file->f_flags & O_NONBLOCK))
		r = wait_event_interruptible(desc->wait, !test_bit(WDM_IN_USE,
								&desc->flags));
	else
		if (test_bit(WDM_IN_USE, &desc->flags))
			r = -EAGAIN;

	if (test_bit(WDM_RESETTING, &desc->flags))
		r = -EIO;

	if (r < 0) {
		kfree(buf);
		rv = r;
		goto out;
	}

	req = desc->orq;
	usb_fill_control_urb(
		desc->command,
		interface_to_usbdev(desc->intf),
		/* using common endpoint 0 */
		usb_sndctrlpipe(interface_to_usbdev(desc->intf), 0),
		(unsigned char *)req,
		buf,
		count,
		wdm_out_callback,
		desc
	);

	req->bRequestType = (USB_DIR_OUT | USB_TYPE_CLASS |
			     USB_RECIP_INTERFACE);
	req->bRequest = USB_CDC_SEND_ENCAPSULATED_COMMAND;
	req->wValue = 0;
	req->wIndex = desc->inum;
	req->wLength = cpu_to_le16(count);
	set_bit(WDM_IN_USE, &desc->flags);
	desc->outbuf = buf;

	rv = usb_submit_urb(desc->command, GFP_KERNEL);
	if (rv < 0) {
		kfree(buf);
		desc->outbuf = NULL;
		clear_bit(WDM_IN_USE, &desc->flags);
		dev_err(&desc->intf->dev, "Tx URB error: %d\n", rv);
		rv = usb_translate_errors(rv);
	} else {
		dev_dbg(&desc->intf->dev, "Tx URB has been submitted index=%d",
			req->wIndex);
	}
out:
	usb_autopm_put_interface(desc->intf);
outnp:
	mutex_unlock(&desc->wlock);
outnl:
	return rv < 0 ? rv : count;
}
