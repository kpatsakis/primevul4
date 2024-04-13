static int edge_write_room(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	int room = 0;
	unsigned long flags;

	if (edge_port == NULL)
		return 0;
	if (edge_port->close_pending == 1)
		return 0;

	spin_lock_irqsave(&edge_port->ep_lock, flags);
	room = kfifo_avail(&edge_port->write_fifo);
	spin_unlock_irqrestore(&edge_port->ep_lock, flags);

	dev_dbg(&port->dev, "%s - returns %d\n", __func__, room);
	return room;
}
