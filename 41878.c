static void edge_unthrottle(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	int status;

	if (edge_port == NULL)
		return;

	/* if we are implementing XON/XOFF, send the start character */
	if (I_IXOFF(tty)) {
		unsigned char start_char = START_CHAR(tty);
		status = edge_write(tty, port, &start_char, 1);
		if (status <= 0) {
			dev_err(&port->dev, "%s - failed to write start character, %d\n", __func__, status);
		}
	}
	/* if we are implementing RTS/CTS, restart reads */
	/* are the Edgeport will assert the RTS line */
	if (C_CRTSCTS(tty)) {
		status = restart_read(edge_port);
		if (status)
			dev_err(&port->dev,
				"%s - read bulk usb_submit_urb failed: %d\n",
							__func__, status);
	}

}
