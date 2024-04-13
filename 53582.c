static int hub_suspend(struct usb_interface *intf, pm_message_t msg)
{
	struct usb_hub		*hub = usb_get_intfdata(intf);
	struct usb_device	*hdev = hub->hdev;
	unsigned		port1;
	int			status;

	/*
	 * Warn if children aren't already suspended.
	 * Also, add up the number of wakeup-enabled descendants.
	 */
	hub->wakeup_enabled_descendants = 0;
	for (port1 = 1; port1 <= hdev->maxchild; port1++) {
		struct usb_port *port_dev = hub->ports[port1 - 1];
		struct usb_device *udev = port_dev->child;

		if (udev && udev->can_submit) {
			dev_warn(&port_dev->dev, "device %s not suspended yet\n",
					dev_name(&udev->dev));
			if (PMSG_IS_AUTO(msg))
				return -EBUSY;
		}
		if (udev)
			hub->wakeup_enabled_descendants +=
					wakeup_enabled_descendants(udev);
	}

	if (hdev->do_remote_wakeup && hub->quirk_check_port_auto_suspend) {
		/* check if there are changes pending on hub ports */
		if (check_ports_changed(hub)) {
			if (PMSG_IS_AUTO(msg))
				return -EBUSY;
			pm_wakeup_event(&hdev->dev, 2000);
		}
	}

	if (hub_is_superspeed(hdev) && hdev->do_remote_wakeup) {
		/* Enable hub to send remote wakeup for all ports. */
		for (port1 = 1; port1 <= hdev->maxchild; port1++) {
			status = set_port_feature(hdev,
					port1 |
					USB_PORT_FEAT_REMOTE_WAKE_CONNECT |
					USB_PORT_FEAT_REMOTE_WAKE_DISCONNECT |
					USB_PORT_FEAT_REMOTE_WAKE_OVER_CURRENT,
					USB_PORT_FEAT_REMOTE_WAKE_MASK);
		}
	}

	dev_dbg(&intf->dev, "%s\n", __func__);

	/* stop hub_wq and related activity */
	hub_quiesce(hub, HUB_SUSPEND);
	return 0;
}
