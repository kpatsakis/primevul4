static int check_ports_changed(struct usb_hub *hub)
{
	int port1;

	for (port1 = 1; port1 <= hub->hdev->maxchild; ++port1) {
		u16 portstatus, portchange;
		int status;

		status = hub_port_status(hub, port1, &portstatus, &portchange);
		if (!status && portchange)
			return 1;
	}
	return 0;
}
