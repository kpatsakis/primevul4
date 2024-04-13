static int edge_chars_in_buffer(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	int chars = 0;
	unsigned long flags;
	if (edge_port == NULL)
		return 0;

	spin_lock_irqsave(&edge_port->ep_lock, flags);
	chars = kfifo_len(&port->write_fifo);
	spin_unlock_irqrestore(&edge_port->ep_lock, flags);

	dev_dbg(&port->dev, "%s - returns %d\n", __func__, chars);
	return chars;
}
