static void whiteheat_close(struct usb_serial_port *port)
{
	firm_report_tx_done(port);
	firm_close(port);

	usb_serial_generic_close(port);

	stop_command_port(port->serial);
}
