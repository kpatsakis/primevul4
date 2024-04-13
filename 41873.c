static int edge_remove_sysfs_attrs(struct usb_serial_port *port)
{
	device_remove_file(&port->dev, &dev_attr_uart_mode);
	return 0;
}
