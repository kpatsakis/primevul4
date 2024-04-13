static int edge_port_remove(struct usb_serial_port *port)
{
	struct edgeport_port *edge_port;

	edge_port = usb_get_serial_port_data(port);

	edge_remove_sysfs_attrs(port);
	kfifo_free(&edge_port->write_fifo);
	kfree(edge_port);

	return 0;
}
