static int digi_write_room(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct digi_port *priv = usb_get_serial_port_data(port);
	int room;
	unsigned long flags = 0;

	spin_lock_irqsave(&priv->dp_port_lock, flags);

	if (priv->dp_write_urb_in_use)
		room = 0;
	else
		room = port->bulk_out_size - 2 - priv->dp_out_buf_len;

	spin_unlock_irqrestore(&priv->dp_port_lock, flags);
	dev_dbg(&port->dev, "digi_write_room: port=%d, room=%d\n", priv->dp_port_num, room);
	return room;

}
