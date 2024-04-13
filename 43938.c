static int firm_close(struct usb_serial_port *port)
{
	struct whiteheat_simple close_command;

	close_command.port = port->port_number + 1;
	return firm_send_command(port, WHITEHEAT_CLOSE,
			(__u8 *)&close_command, sizeof(close_command));
}
