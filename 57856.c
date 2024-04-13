static int purge_port(struct usb_serial_port *port, __u16 mask)
{
	int port_number = port->port_number;

	dev_dbg(&port->dev, "%s - port %d, mask %x\n", __func__, port_number, mask);

	return send_cmd(port->serial->dev,
					UMPC_PURGE_PORT,
					(__u8)(UMPM_UART1_PORT + port_number),
					mask,
					NULL,
					0);
}
