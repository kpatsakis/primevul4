static int gs_usb_set_bittiming(struct net_device *netdev)
{
	struct gs_can *dev = netdev_priv(netdev);
	struct can_bittiming *bt = &dev->can.bittiming;
	struct usb_interface *intf = dev->iface;
	int rc;
	struct gs_device_bittiming *dbt;

	dbt = kmalloc(sizeof(*dbt), GFP_KERNEL);
	if (!dbt)
		return -ENOMEM;

	dbt->prop_seg = bt->prop_seg;
	dbt->phase_seg1 = bt->phase_seg1;
	dbt->phase_seg2 = bt->phase_seg2;
	dbt->sjw = bt->sjw;
	dbt->brp = bt->brp;

	/* request bit timings */
	rc = usb_control_msg(interface_to_usbdev(intf),
			     usb_sndctrlpipe(interface_to_usbdev(intf), 0),
			     GS_USB_BREQ_BITTIMING,
			     USB_DIR_OUT|USB_TYPE_VENDOR|USB_RECIP_INTERFACE,
			     dev->channel,
			     0,
			     dbt,
			     sizeof(*dbt),
			     1000);

	kfree(dbt);

	if (rc < 0)
		dev_err(netdev->dev.parent, "Couldn't set bittimings (err=%d)",
			rc);

	return rc;
}
