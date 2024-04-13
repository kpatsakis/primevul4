static ssize_t port_fops_read(struct file *filp, char __user *ubuf,
			      size_t count, loff_t *offp)
{
	struct port *port;
	ssize_t ret;

	port = filp->private_data;

	/* Port is hot-unplugged. */
	if (!port->guest_connected)
		return -ENODEV;

	if (!port_has_data(port)) {
		/*
		 * If nothing's connected on the host just return 0 in
		 * case of list_empty; this tells the userspace app
		 * that there's no connection
		 */
		if (!port->host_connected)
			return 0;
		if (filp->f_flags & O_NONBLOCK)
			return -EAGAIN;

		ret = wait_event_freezable(port->waitqueue,
					   !will_read_block(port));
		if (ret < 0)
			return ret;
	}
	/* Port got hot-unplugged while we were waiting above. */
	if (!port->guest_connected)
		return -ENODEV;
	/*
	 * We could've received a disconnection message while we were
	 * waiting for more data.
	 *
	 * This check is not clubbed in the if() statement above as we
	 * might receive some data as well as the host could get
	 * disconnected after we got woken up from our wait.  So we
	 * really want to give off whatever data we have and only then
	 * check for host_connected.
	 */
	if (!port_has_data(port) && !port->host_connected)
		return 0;

	return fill_readbuf(port, ubuf, count, true);
}
