static int proc_disconnect_claim(struct usb_dev_state *ps, void __user *arg)
{
	struct usbdevfs_disconnect_claim dc;
	struct usb_interface *intf;

	if (copy_from_user(&dc, arg, sizeof(dc)))
		return -EFAULT;

	intf = usb_ifnum_to_if(ps->dev, dc.interface);
	if (!intf)
		return -EINVAL;

	if (intf->dev.driver) {
		struct usb_driver *driver = to_usb_driver(intf->dev.driver);

		if (ps->privileges_dropped)
			return -EACCES;

		if ((dc.flags & USBDEVFS_DISCONNECT_CLAIM_IF_DRIVER) &&
				strncmp(dc.driver, intf->dev.driver->name,
					sizeof(dc.driver)) != 0)
			return -EBUSY;

		if ((dc.flags & USBDEVFS_DISCONNECT_CLAIM_EXCEPT_DRIVER) &&
				strncmp(dc.driver, intf->dev.driver->name,
					sizeof(dc.driver)) == 0)
			return -EBUSY;

		dev_dbg(&intf->dev, "disconnect by usbfs\n");
		usb_driver_release_interface(driver, intf);
	}

	return claimintf(ps, dc.interface);
}
