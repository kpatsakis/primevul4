static int mct_u232_port_remove(struct usb_serial_port *port)
{
	struct mct_u232_private *priv;

	priv = usb_get_serial_port_data(port);
	kfree(priv);

	return 0;
}
