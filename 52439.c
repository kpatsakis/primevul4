static int digi_open(struct tty_struct *tty, struct usb_serial_port *port)
{
	int ret;
	unsigned char buf[32];
	struct digi_port *priv = usb_get_serial_port_data(port);
	struct ktermios not_termios;

	/* be sure the device is started up */
	if (digi_startup_device(port->serial) != 0)
		return -ENXIO;

	/* read modem signals automatically whenever they change */
	buf[0] = DIGI_CMD_READ_INPUT_SIGNALS;
	buf[1] = priv->dp_port_num;
	buf[2] = DIGI_ENABLE;
	buf[3] = 0;

	/* flush fifos */
	buf[4] = DIGI_CMD_IFLUSH_FIFO;
	buf[5] = priv->dp_port_num;
	buf[6] = DIGI_FLUSH_TX | DIGI_FLUSH_RX;
	buf[7] = 0;

	ret = digi_write_oob_command(port, buf, 8, 1);
	if (ret != 0)
		dev_dbg(&port->dev, "digi_open: write oob failed, ret=%d\n", ret);

	/* set termios settings */
	if (tty) {
		not_termios.c_cflag = ~tty->termios.c_cflag;
		not_termios.c_iflag = ~tty->termios.c_iflag;
		digi_set_termios(tty, port, &not_termios);
	}
	return 0;
}
