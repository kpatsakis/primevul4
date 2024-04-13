static int whiteheat_port_remove(struct usb_serial_port *port)
{
	struct whiteheat_private *info;

	info = usb_get_serial_port_data(port);
	kfree(info);

	return 0;
}
