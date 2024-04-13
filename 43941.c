static int firm_purge(struct usb_serial_port *port, __u8 rxtx)
{
	struct whiteheat_purge purge_command;

	purge_command.port = port->port_number + 1;
	purge_command.what = rxtx;
	return firm_send_command(port, WHITEHEAT_PURGE,
			(__u8 *)&purge_command, sizeof(purge_command));
}
