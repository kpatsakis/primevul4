static int omninet_write_room(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct usb_serial 	*serial = port->serial;
	struct usb_serial_port 	*wport 	= serial->port[1];

	int room = 0; /* Default: no room */

	if (test_bit(0, &wport->write_urbs_free))
		room = wport->bulk_out_size - OMNINET_HEADERLEN;

	dev_dbg(&port->dev, "%s - returns %d\n", __func__, room);

	return room;
}
