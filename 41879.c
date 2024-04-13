static int edge_write(struct tty_struct *tty, struct usb_serial_port *port,
				const unsigned char *data, int count)
{
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);

	if (count == 0) {
		dev_dbg(&port->dev, "%s - write request of 0 bytes\n", __func__);
		return 0;
	}

	if (edge_port == NULL)
		return -ENODEV;
	if (edge_port->close_pending == 1)
		return -ENODEV;

	count = kfifo_in_locked(&edge_port->write_fifo, data, count,
							&edge_port->ep_lock);
	edge_send(tty);

	return count;
}
