static bool edge_tx_empty(struct usb_serial_port *port)
{
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	int ret;

	ret = tx_active(edge_port);
	if (ret > 0)
		return false;

	return true;
}
