static int whiteheat_port_probe(struct usb_serial_port *port)
{
	struct whiteheat_private *info;

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	usb_set_serial_port_data(port, info);

	return 0;
}
