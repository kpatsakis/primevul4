static void digi_dtr_rts(struct usb_serial_port *port, int on)
{
	/* Adjust DTR and RTS */
	digi_set_modem_signals(port, on * (TIOCM_DTR|TIOCM_RTS), 1);
}
