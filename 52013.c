static int proc_control(struct usb_dev_state *ps, void __user *arg)
{
	struct usb_device *dev = ps->dev;
	struct usbdevfs_ctrltransfer ctrl;
	unsigned int tmo;
	unsigned char *tbuf;
	unsigned wLength;
	int i, pipe, ret;

	if (copy_from_user(&ctrl, arg, sizeof(ctrl)))
		return -EFAULT;
	ret = check_ctrlrecip(ps, ctrl.bRequestType, ctrl.bRequest,
			      ctrl.wIndex);
	if (ret)
		return ret;
	wLength = ctrl.wLength;		/* To suppress 64k PAGE_SIZE warning */
	if (wLength > PAGE_SIZE)
		return -EINVAL;
	ret = usbfs_increase_memory_usage(PAGE_SIZE + sizeof(struct urb) +
			sizeof(struct usb_ctrlrequest));
	if (ret)
		return ret;
	tbuf = (unsigned char *)__get_free_page(GFP_KERNEL);
	if (!tbuf) {
		ret = -ENOMEM;
		goto done;
	}
	tmo = ctrl.timeout;
	snoop(&dev->dev, "control urb: bRequestType=%02x "
		"bRequest=%02x wValue=%04x "
		"wIndex=%04x wLength=%04x\n",
		ctrl.bRequestType, ctrl.bRequest, ctrl.wValue,
		ctrl.wIndex, ctrl.wLength);
	if (ctrl.bRequestType & 0x80) {
		if (ctrl.wLength && !access_ok(VERIFY_WRITE, ctrl.data,
					       ctrl.wLength)) {
			ret = -EINVAL;
			goto done;
		}
		pipe = usb_rcvctrlpipe(dev, 0);
		snoop_urb(dev, NULL, pipe, ctrl.wLength, tmo, SUBMIT, NULL, 0);

		usb_unlock_device(dev);
		i = usb_control_msg(dev, pipe, ctrl.bRequest,
				    ctrl.bRequestType, ctrl.wValue, ctrl.wIndex,
				    tbuf, ctrl.wLength, tmo);
		usb_lock_device(dev);
		snoop_urb(dev, NULL, pipe, max(i, 0), min(i, 0), COMPLETE,
			  tbuf, max(i, 0));
		if ((i > 0) && ctrl.wLength) {
			if (copy_to_user(ctrl.data, tbuf, i)) {
				ret = -EFAULT;
				goto done;
			}
		}
	} else {
		if (ctrl.wLength) {
			if (copy_from_user(tbuf, ctrl.data, ctrl.wLength)) {
				ret = -EFAULT;
				goto done;
			}
		}
		pipe = usb_sndctrlpipe(dev, 0);
		snoop_urb(dev, NULL, pipe, ctrl.wLength, tmo, SUBMIT,
			tbuf, ctrl.wLength);

		usb_unlock_device(dev);
		i = usb_control_msg(dev, usb_sndctrlpipe(dev, 0), ctrl.bRequest,
				    ctrl.bRequestType, ctrl.wValue, ctrl.wIndex,
				    tbuf, ctrl.wLength, tmo);
		usb_lock_device(dev);
		snoop_urb(dev, NULL, pipe, max(i, 0), min(i, 0), COMPLETE, NULL, 0);
	}
	if (i < 0 && i != -EPIPE) {
		dev_printk(KERN_DEBUG, &dev->dev, "usbfs: USBDEVFS_CONTROL "
			   "failed cmd %s rqt %u rq %u len %u ret %d\n",
			   current->comm, ctrl.bRequestType, ctrl.bRequest,
			   ctrl.wLength, i);
	}
	ret = i;
 done:
	free_page((unsigned long) tbuf);
	usbfs_decrease_memory_usage(PAGE_SIZE + sizeof(struct urb) +
			sizeof(struct usb_ctrlrequest));
	return ret;
}
