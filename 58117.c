static int wait_port_writable(struct port *port, bool nonblock)
{
	int ret;

	if (will_write_block(port)) {
		if (nonblock)
			return -EAGAIN;

		ret = wait_event_freezable(port->waitqueue,
					   !will_write_block(port));
		if (ret < 0)
			return ret;
	}
	/* Port got hot-unplugged. */
	if (!port->guest_connected)
		return -ENODEV;

	return 0;
}
