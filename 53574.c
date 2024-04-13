static int hub_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
	struct usb_host_interface *desc;
	struct usb_endpoint_descriptor *endpoint;
	struct usb_device *hdev;
	struct usb_hub *hub;

	desc = intf->cur_altsetting;
	hdev = interface_to_usbdev(intf);

	/*
	 * Set default autosuspend delay as 0 to speedup bus suspend,
	 * based on the below considerations:
	 *
	 * - Unlike other drivers, the hub driver does not rely on the
	 *   autosuspend delay to provide enough time to handle a wakeup
	 *   event, and the submitted status URB is just to check future
	 *   change on hub downstream ports, so it is safe to do it.
	 *
	 * - The patch might cause one or more auto supend/resume for
	 *   below very rare devices when they are plugged into hub
	 *   first time:
	 *
	 *   	devices having trouble initializing, and disconnect
	 *   	themselves from the bus and then reconnect a second
	 *   	or so later
	 *
	 *   	devices just for downloading firmware, and disconnects
	 *   	themselves after completing it
	 *
	 *   For these quite rare devices, their drivers may change the
	 *   autosuspend delay of their parent hub in the probe() to one
	 *   appropriate value to avoid the subtle problem if someone
	 *   does care it.
	 *
	 * - The patch may cause one or more auto suspend/resume on
	 *   hub during running 'lsusb', but it is probably too
	 *   infrequent to worry about.
	 *
	 * - Change autosuspend delay of hub can avoid unnecessary auto
	 *   suspend timer for hub, also may decrease power consumption
	 *   of USB bus.
	 *
	 * - If user has indicated to prevent autosuspend by passing
	 *   usbcore.autosuspend = -1 then keep autosuspend disabled.
	 */
#ifdef CONFIG_PM
	if (hdev->dev.power.autosuspend_delay >= 0)
		pm_runtime_set_autosuspend_delay(&hdev->dev, 0);
#endif

	/*
	 * Hubs have proper suspend/resume support, except for root hubs
	 * where the controller driver doesn't have bus_suspend and
	 * bus_resume methods.
	 */
	if (hdev->parent) {		/* normal device */
		usb_enable_autosuspend(hdev);
	} else {			/* root hub */
		const struct hc_driver *drv = bus_to_hcd(hdev->bus)->driver;

		if (drv->bus_suspend && drv->bus_resume)
			usb_enable_autosuspend(hdev);
	}

	if (hdev->level == MAX_TOPO_LEVEL) {
		dev_err(&intf->dev,
			"Unsupported bus topology: hub nested too deep\n");
		return -E2BIG;
	}

#ifdef	CONFIG_USB_OTG_BLACKLIST_HUB
	if (hdev->parent) {
		dev_warn(&intf->dev, "ignoring external hub\n");
		return -ENODEV;
	}
#endif

	/* Some hubs have a subclass of 1, which AFAICT according to the */
	/*  specs is not defined, but it works */
	if ((desc->desc.bInterfaceSubClass != 0) &&
	    (desc->desc.bInterfaceSubClass != 1)) {
descriptor_error:
		dev_err(&intf->dev, "bad descriptor, ignoring hub\n");
		return -EIO;
	}

	/* Multiple endpoints? What kind of mutant ninja-hub is this? */
	if (desc->desc.bNumEndpoints != 1)
		goto descriptor_error;

	endpoint = &desc->endpoint[0].desc;

	/* If it's not an interrupt in endpoint, we'd better punt! */
	if (!usb_endpoint_is_int_in(endpoint))
		goto descriptor_error;

	/* We found a hub */
	dev_info(&intf->dev, "USB hub found\n");

	hub = kzalloc(sizeof(*hub), GFP_KERNEL);
	if (!hub) {
		dev_dbg(&intf->dev, "couldn't kmalloc hub struct\n");
		return -ENOMEM;
	}

	kref_init(&hub->kref);
	hub->intfdev = &intf->dev;
	hub->hdev = hdev;
	INIT_DELAYED_WORK(&hub->leds, led_work);
	INIT_DELAYED_WORK(&hub->init_work, NULL);
	INIT_WORK(&hub->events, hub_event);
	usb_get_intf(intf);
	usb_get_dev(hdev);

	usb_set_intfdata(intf, hub);
	intf->needs_remote_wakeup = 1;
	pm_suspend_ignore_children(&intf->dev, true);

	if (hdev->speed == USB_SPEED_HIGH)
		highspeed_hubs++;

	if (id->driver_info & HUB_QUIRK_CHECK_PORT_AUTOSUSPEND)
		hub->quirk_check_port_auto_suspend = 1;

	if (hub_configure(hub, endpoint) >= 0)
		return 0;

	hub_disconnect(intf);
	return -ENODEV;
}
