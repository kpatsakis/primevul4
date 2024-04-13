static int firm_set_rts(struct usb_serial_port *port, __u8 onoff)
{
	struct whiteheat_set_rdb rts_command;

	rts_command.port = port->port_number + 1;
	rts_command.state = onoff;
	return firm_send_command(port, WHITEHEAT_SET_RTS,
			(__u8 *)&rts_command, sizeof(rts_command));
}
