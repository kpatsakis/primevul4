static int edge_resume(struct usb_serial *serial)
{
	struct edgeport_serial *edge_serial = usb_get_serial_data(serial);

	edge_heartbeat_schedule(edge_serial);

	return 0;
}
