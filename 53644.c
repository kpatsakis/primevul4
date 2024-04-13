static int wait_for_ss_port_enable(struct usb_device *udev,
		struct usb_hub *hub, int *port1,
		u16 *portchange, u16 *portstatus)
{
	int status = 0, delay_ms = 0;

	while (delay_ms < 2000) {
		if (status || *portstatus & USB_PORT_STAT_CONNECTION)
			break;
		msleep(20);
		delay_ms += 20;
		status = hub_port_status(hub, *port1, portstatus, portchange);
	}
	return status;
}
