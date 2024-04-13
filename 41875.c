static void edge_throttle(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	int status;

	if (edge_port == NULL)
		return;

	/* if we are implementing XON/XOFF, send the stop character */
	if (I_IXOFF(tty)) {
		unsigned char stop_char = STOP_CHAR(tty);
		status = edge_write(tty, port, &stop_char, 1);
		if (status <= 0) {
			dev_err(&port->dev, "%s - failed to write stop character, %d\n", __func__, status);
		}
	}

	/* if we are implementing RTS/CTS, stop reads */
	/* and the Edgeport will clear the RTS line */
	if (C_CRTSCTS(tty))
		stop_read(edge_port);

}
