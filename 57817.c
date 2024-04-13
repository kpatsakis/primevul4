static int omninet_port_remove(struct usb_serial_port *port)
{
	struct omninet_data *od;

	od = usb_get_serial_port_data(port);
	kfree(od);

	return 0;
}
