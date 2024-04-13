static void usb_set_lpm_parameters(struct usb_device *udev)
{
	struct usb_hub *hub;
	unsigned int port_to_port_delay;
	unsigned int udev_u1_del;
	unsigned int udev_u2_del;
	unsigned int hub_u1_del;
	unsigned int hub_u2_del;

	if (!udev->lpm_capable || udev->speed != USB_SPEED_SUPER)
		return;

	hub = usb_hub_to_struct_hub(udev->parent);
	/* It doesn't take time to transition the roothub into U0, since it
	 * doesn't have an upstream link.
	 */
	if (!hub)
		return;

	udev_u1_del = udev->bos->ss_cap->bU1devExitLat;
	udev_u2_del = le16_to_cpu(udev->bos->ss_cap->bU2DevExitLat);
	hub_u1_del = udev->parent->bos->ss_cap->bU1devExitLat;
	hub_u2_del = le16_to_cpu(udev->parent->bos->ss_cap->bU2DevExitLat);

	usb_set_lpm_mel(udev, &udev->u1_params, udev_u1_del,
			hub, &udev->parent->u1_params, hub_u1_del);

	usb_set_lpm_mel(udev, &udev->u2_params, udev_u2_del,
			hub, &udev->parent->u2_params, hub_u2_del);

	/*
	 * Appendix C, section C.2.2.2, says that there is a slight delay from
	 * when the parent hub notices the downstream port is trying to
	 * transition to U0 to when the hub initiates a U0 transition on its
	 * upstream port.  The section says the delays are tPort2PortU1EL and
	 * tPort2PortU2EL, but it doesn't define what they are.
	 *
	 * The hub chapter, sections 10.4.2.4 and 10.4.2.5 seem to be talking
	 * about the same delays.  Use the maximum delay calculations from those
	 * sections.  For U1, it's tHubPort2PortExitLat, which is 1us max.  For
	 * U2, it's tHubPort2PortExitLat + U2DevExitLat - U1DevExitLat.  I
	 * assume the device exit latencies they are talking about are the hub
	 * exit latencies.
	 *
	 * What do we do if the U2 exit latency is less than the U1 exit
	 * latency?  It's possible, although not likely...
	 */
	port_to_port_delay = 1;

	usb_set_lpm_pel(udev, &udev->u1_params, udev_u1_del,
			hub, &udev->parent->u1_params, hub_u1_del,
			port_to_port_delay);

	if (hub_u2_del > hub_u1_del)
		port_to_port_delay = 1 + hub_u2_del - hub_u1_del;
	else
		port_to_port_delay = 1 + hub_u1_del;

	usb_set_lpm_pel(udev, &udev->u2_params, udev_u2_del,
			hub, &udev->parent->u2_params, hub_u2_del,
			port_to_port_delay);

	/* Now that we've got PEL, calculate SEL. */
	usb_set_lpm_sel(udev, &udev->u1_params);
	usb_set_lpm_sel(udev, &udev->u2_params);
}
