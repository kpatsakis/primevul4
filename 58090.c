static bool is_console_port(struct port *port)
{
	if (port->cons.hvc)
		return true;
	return false;
}
