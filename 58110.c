static void set_console_size(struct port *port, u16 rows, u16 cols)
{
	if (!port || !is_console_port(port))
		return;

	port->cons.ws.ws_row = rows;
	port->cons.ws.ws_col = cols;
}
