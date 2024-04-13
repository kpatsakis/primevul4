static bool port_has_data(struct port *port)
{
	unsigned long flags;
	bool ret;

	ret = false;
	spin_lock_irqsave(&port->inbuf_lock, flags);
	port->inbuf = get_inbuf(port);
	if (port->inbuf)
		ret = true;

	spin_unlock_irqrestore(&port->inbuf_lock, flags);
	return ret;
}
