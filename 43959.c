static void whiteheat_set_termios(struct tty_struct *tty,
	struct usb_serial_port *port, struct ktermios *old_termios)
{
	firm_setup_port(tty);
}
