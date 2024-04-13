static int omninet_port_probe(struct usb_serial_port *port)
{
	struct omninet_data *od;

	od = kzalloc(sizeof(*od), GFP_KERNEL);
	if (!od)
		return -ENOMEM;

	usb_set_serial_port_data(port, od);

	return 0;
}
