static int klsi_105_port_remove(struct usb_serial_port *port)
{
	struct klsi_105_private *priv;

	priv = usb_get_serial_port_data(port);
	kfree(priv);

	return 0;
}
