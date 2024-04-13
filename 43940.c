static int firm_open(struct usb_serial_port *port)
{
	struct whiteheat_simple open_command;

	open_command.port = port->port_number + 1;
	return firm_send_command(port, WHITEHEAT_OPEN,
		(__u8 *)&open_command, sizeof(open_command));
}
