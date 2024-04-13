static int claimintf(struct usb_dev_state *ps, unsigned int ifnum)
{
	struct usb_device *dev = ps->dev;
	struct usb_interface *intf;
	int err;

	if (ifnum >= 8*sizeof(ps->ifclaimed))
		return -EINVAL;
	/* already claimed */
	if (test_bit(ifnum, &ps->ifclaimed))
		return 0;

	if (ps->privileges_dropped &&
			!test_bit(ifnum, &ps->interface_allowed_mask))
		return -EACCES;

	intf = usb_ifnum_to_if(dev, ifnum);
	if (!intf)
		err = -ENOENT;
	else
		err = usb_driver_claim_interface(&usbfs_driver, intf, ps);
	if (err == 0)
		set_bit(ifnum, &ps->ifclaimed);
	return err;
}
