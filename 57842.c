static void edge_set_termios(struct tty_struct *tty,
		struct usb_serial_port *port, struct ktermios *old_termios)
{
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	unsigned int cflag;

	cflag = tty->termios.c_cflag;

	dev_dbg(&port->dev, "%s - clfag %08x iflag %08x\n", __func__,
		tty->termios.c_cflag, tty->termios.c_iflag);
	dev_dbg(&port->dev, "%s - old clfag %08x old iflag %08x\n", __func__,
		old_termios->c_cflag, old_termios->c_iflag);

	if (edge_port == NULL)
		return;
	/* change the port settings to the new ones specified */
	change_port_settings(tty, edge_port, old_termios);
}
