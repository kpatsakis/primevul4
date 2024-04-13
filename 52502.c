static int cypress_ca42v2_port_probe(struct usb_serial_port *port)
{
	struct cypress_private *priv;
	int ret;

	ret = cypress_generic_port_probe(port);
	if (ret) {
		dev_dbg(&port->dev, "%s - Failed setting up port\n", __func__);
		return ret;
	}

	priv = usb_get_serial_port_data(port);
	priv->chiptype = CT_CA42V2;

	return 0;
}
