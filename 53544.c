static int check_port_resume_type(struct usb_device *udev,
		struct usb_hub *hub, int port1,
		int status, u16 portchange, u16 portstatus)
{
	struct usb_port *port_dev = hub->ports[port1 - 1];
	int retries = 3;

 retry:
	/* Is a warm reset needed to recover the connection? */
	if (status == 0 && udev->reset_resume
		&& hub_port_warm_reset_required(hub, port1, portstatus)) {
		/* pass */;
	}
	/* Is the device still present? */
	else if (status || port_is_suspended(hub, portstatus) ||
			!port_is_power_on(hub, portstatus)) {
		if (status >= 0)
			status = -ENODEV;
	} else if (!(portstatus & USB_PORT_STAT_CONNECTION)) {
		if (retries--) {
			usleep_range(200, 300);
			status = hub_port_status(hub, port1, &portstatus,
							     &portchange);
			goto retry;
		}
		status = -ENODEV;
	}

	/* Can't do a normal resume if the port isn't enabled,
	 * so try a reset-resume instead.
	 */
	else if (!(portstatus & USB_PORT_STAT_ENABLE) && !udev->reset_resume) {
		if (udev->persist_enabled)
			udev->reset_resume = 1;
		else
			status = -ENODEV;
	}

	if (status) {
		dev_dbg(&port_dev->dev, "status %04x.%04x after resume, %d\n",
				portchange, portstatus, status);
	} else if (udev->reset_resume) {

		/* Late port handoff can set status-change bits */
		if (portchange & USB_PORT_STAT_C_CONNECTION)
			usb_clear_port_feature(hub->hdev, port1,
					USB_PORT_FEAT_C_CONNECTION);
		if (portchange & USB_PORT_STAT_C_ENABLE)
			usb_clear_port_feature(hub->hdev, port1,
					USB_PORT_FEAT_C_ENABLE);
	}

	return status;
}
