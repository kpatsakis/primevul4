static int ti_do_config(struct edgeport_port *port, int feature, int on)
{
	int port_number = port->port->number - port->port->serial->minor;
	on = !!on;	/* 1 or 0 not bitmask */
	return send_cmd(port->port->serial->dev,
			feature, (__u8)(UMPM_UART1_PORT + port_number),
			on, NULL, 0);
}
