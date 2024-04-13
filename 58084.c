static int get_chars(u32 vtermno, char *buf, int count)
{
	struct port *port;

	/* If we've not set up the port yet, we have no input to give. */
	if (unlikely(early_put_chars))
		return 0;

	port = find_port_by_vtermno(vtermno);
	if (!port)
		return -EPIPE;

	/* If we don't have an input queue yet, we can't get input. */
	BUG_ON(!port->in_vq);

	return fill_readbuf(port, (__force char __user *)buf, count, false);
}
