static unsigned int port_fops_poll(struct file *filp, poll_table *wait)
{
	struct port *port;
	unsigned int ret;

	port = filp->private_data;
	poll_wait(filp, &port->waitqueue, wait);

	if (!port->guest_connected) {
		/* Port got unplugged */
		return POLLHUP;
	}
	ret = 0;
	if (!will_read_block(port))
		ret |= POLLIN | POLLRDNORM;
	if (!will_write_block(port))
		ret |= POLLOUT;
	if (!port->host_connected)
		ret |= POLLHUP;

	return ret;
}
