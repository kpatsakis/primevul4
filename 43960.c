static int whiteheat_tiocmget(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct whiteheat_private *info = usb_get_serial_port_data(port);
	unsigned int modem_signals = 0;

	firm_get_dtr_rts(port);
	if (info->mcr & UART_MCR_DTR)
		modem_signals |= TIOCM_DTR;
	if (info->mcr & UART_MCR_RTS)
		modem_signals |= TIOCM_RTS;

	return modem_signals;
}
