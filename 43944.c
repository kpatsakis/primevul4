static int firm_set_break(struct usb_serial_port *port, __u8 onoff)
{
	struct whiteheat_set_rdb break_command;

	break_command.port = port->port_number + 1;
	break_command.state = onoff;
	return firm_send_command(port, WHITEHEAT_SET_BREAK,
			(__u8 *)&break_command, sizeof(break_command));
}
