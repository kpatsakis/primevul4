static int edge_suspend(struct usb_serial *serial, pm_message_t message)
{
	struct edgeport_serial *edge_serial = usb_get_serial_data(serial);

	cancel_delayed_work_sync(&edge_serial->heartbeat_work);

	return 0;
}
