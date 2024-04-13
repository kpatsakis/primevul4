static int firm_get_dtr_rts(struct usb_serial_port *port)
{
	struct whiteheat_simple get_dr_command;

	get_dr_command.port = port->port_number + 1;
	return firm_send_command(port, WHITEHEAT_GET_DTR_RTS,
			(__u8 *)&get_dr_command, sizeof(get_dr_command));
}
