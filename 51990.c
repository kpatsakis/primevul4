static int check_ctrlrecip(struct usb_dev_state *ps, unsigned int requesttype,
			   unsigned int request, unsigned int index)
{
	int ret = 0;
	struct usb_host_interface *alt_setting;

	if (ps->dev->state != USB_STATE_UNAUTHENTICATED
	 && ps->dev->state != USB_STATE_ADDRESS
	 && ps->dev->state != USB_STATE_CONFIGURED)
		return -EHOSTUNREACH;
	if (USB_TYPE_VENDOR == (USB_TYPE_MASK & requesttype))
		return 0;

	/*
	 * check for the special corner case 'get_device_id' in the printer
	 * class specification, which we always want to allow as it is used
	 * to query things like ink level, etc.
	 */
	if (requesttype == 0xa1 && request == 0) {
		alt_setting = usb_find_alt_setting(ps->dev->actconfig,
						   index >> 8, index & 0xff);
		if (alt_setting
		 && alt_setting->desc.bInterfaceClass == USB_CLASS_PRINTER)
			return 0;
	}

	index &= 0xff;
	switch (requesttype & USB_RECIP_MASK) {
	case USB_RECIP_ENDPOINT:
		if ((index & ~USB_DIR_IN) == 0)
			return 0;
		ret = findintfep(ps->dev, index);
		if (ret < 0) {
			/*
			 * Some not fully compliant Win apps seem to get
			 * index wrong and have the endpoint number here
			 * rather than the endpoint address (with the
			 * correct direction). Win does let this through,
			 * so we'll not reject it here but leave it to
			 * the device to not break KVM. But we warn.
			 */
			ret = findintfep(ps->dev, index ^ 0x80);
			if (ret >= 0)
				dev_info(&ps->dev->dev,
					"%s: process %i (%s) requesting ep %02x but needs %02x\n",
					__func__, task_pid_nr(current),
					current->comm, index, index ^ 0x80);
		}
		if (ret >= 0)
			ret = checkintf(ps, ret);
		break;

	case USB_RECIP_INTERFACE:
		ret = checkintf(ps, index);
		break;
	}
	return ret;
}
