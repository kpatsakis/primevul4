static int firm_report_tx_done(struct usb_serial_port *port)
{
	struct whiteheat_simple close_command;

	close_command.port = port->port_number + 1;
	return firm_send_command(port, WHITEHEAT_REPORT_TX_DONE,
			(__u8 *)&close_command, sizeof(close_command));
}
