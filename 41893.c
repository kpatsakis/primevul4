static ssize_t show_uart_mode(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct usb_serial_port *port = to_usb_serial_port(dev);
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);

	return sprintf(buf, "%d\n", edge_port->bUartMode);
}
