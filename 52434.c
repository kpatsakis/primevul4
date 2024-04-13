static void digi_break_ctl(struct tty_struct *tty, int break_state)
{
	struct usb_serial_port *port = tty->driver_data;
	unsigned char buf[4];

	buf[0] = DIGI_CMD_BREAK_CONTROL;
	buf[1] = 2;				/* length */
	buf[2] = break_state ? 1 : 0;
	buf[3] = 0;				/* pad */
	digi_write_inb_command(port, buf, 4, 0);
}
