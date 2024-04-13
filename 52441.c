static int digi_port_remove(struct usb_serial_port *port)
{
	struct digi_port *priv;

	priv = usb_get_serial_port_data(port);
	kfree(priv);

	return 0;
}
