static int firm_set_dtr(struct usb_serial_port *port, __u8 onoff)
{
	struct whiteheat_set_rdb dtr_command;

	dtr_command.port = port->port_number + 1;
	dtr_command.state = onoff;
	return firm_send_command(port, WHITEHEAT_SET_DTR,
			(__u8 *)&dtr_command, sizeof(dtr_command));
}
