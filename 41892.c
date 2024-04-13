static int restore_mcr(struct edgeport_port *port, __u8 mcr)
{
	int status = 0;

	dev_dbg(&port->port->dev, "%s - %x\n", __func__, mcr);

	status = ti_do_config(port, UMPC_SET_CLR_DTR, mcr & MCR_DTR);
	if (status)
		return status;
	status = ti_do_config(port, UMPC_SET_CLR_RTS, mcr & MCR_RTS);
	if (status)
		return status;
	return ti_do_config(port, UMPC_SET_CLR_LOOPBACK, mcr & MCR_LOOPBACK);
}
