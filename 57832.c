static void edge_disconnect(struct usb_serial *serial)
{
	struct edgeport_serial *edge_serial = usb_get_serial_data(serial);

	cancel_delayed_work_sync(&edge_serial->heartbeat_work);
}
