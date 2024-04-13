static void whiteheat_break_ctl(struct tty_struct *tty, int break_state)
{
	struct usb_serial_port *port = tty->driver_data;
	firm_set_break(port, break_state);
}
