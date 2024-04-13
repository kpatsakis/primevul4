static int cypress_port_remove(struct usb_serial_port *port)
{
	struct cypress_private *priv;

	priv = usb_get_serial_port_data(port);

	kfifo_free(&priv->write_fifo);
	kfree(priv);

	return 0;
}
