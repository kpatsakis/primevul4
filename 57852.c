static int get_serial_info(struct edgeport_port *edge_port,
				struct serial_struct __user *retinfo)
{
	struct serial_struct tmp;
	unsigned cwait;

	cwait = edge_port->port->port.closing_wait;
	if (cwait != ASYNC_CLOSING_WAIT_NONE)
		cwait = jiffies_to_msecs(cwait) / 10;

	memset(&tmp, 0, sizeof(tmp));

	tmp.type		= PORT_16550A;
	tmp.line		= edge_port->port->minor;
	tmp.port		= edge_port->port->port_number;
	tmp.irq			= 0;
	tmp.xmit_fifo_size	= edge_port->port->bulk_out_size;
	tmp.baud_base		= 9600;
	tmp.close_delay		= 5*HZ;
	tmp.closing_wait	= cwait;

	if (copy_to_user(retinfo, &tmp, sizeof(*retinfo)))
		return -EFAULT;
	return 0;
}
