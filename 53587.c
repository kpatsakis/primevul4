static int port_is_power_on(struct usb_hub *hub, unsigned portstatus)
{
	int ret = 0;

	if (hub_is_superspeed(hub->hdev)) {
		if (portstatus & USB_SS_PORT_STAT_POWER)
			ret = 1;
	} else {
		if (portstatus & USB_PORT_STAT_POWER)
			ret = 1;
	}

	return ret;
}
