static void edge_release(struct usb_serial *serial)
{
	struct edgeport_serial *edge_serial = usb_get_serial_data(serial);

	cancel_delayed_work_sync(&edge_serial->heartbeat_work);
	kfree(edge_serial);
}
