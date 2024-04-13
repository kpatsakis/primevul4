static inline char *portspeed(struct usb_hub *hub, int portstatus)
{
	if (hub_is_superspeed(hub->hdev))
		return "5.0 Gb/s";
	if (portstatus & USB_PORT_STAT_HIGH_SPEED)
		return "480 Mb/s";
	else if (portstatus & USB_PORT_STAT_LOW_SPEED)
		return "1.5 Mb/s";
	else
		return "12 Mb/s";
}
