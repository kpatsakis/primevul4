static void in_intr(struct virtqueue *vq)
{
	struct port *port;
	unsigned long flags;

	port = find_port_by_vq(vq->vdev->priv, vq);
	if (!port)
		return;

	spin_lock_irqsave(&port->inbuf_lock, flags);
	port->inbuf = get_inbuf(port);

	/*
	 * Normally the port should not accept data when the port is
	 * closed. For generic serial ports, the host won't (shouldn't)
	 * send data till the guest is connected. But this condition
	 * can be reached when a console port is not yet connected (no
	 * tty is spawned) and the other side sends out data over the
	 * vring, or when a remote devices start sending data before
	 * the ports are opened.
	 *
	 * A generic serial port will discard data if not connected,
	 * while console ports and rproc-serial ports accepts data at
	 * any time. rproc-serial is initiated with guest_connected to
	 * false because port_fops_open expects this. Console ports are
	 * hooked up with an HVC console and is initialized with
	 * guest_connected to true.
	 */

	if (!port->guest_connected && !is_rproc_serial(port->portdev->vdev))
		discard_port_data(port);

	/* Send a SIGIO indicating new data in case the process asked for it */
	send_sigio_to_port(port);

	spin_unlock_irqrestore(&port->inbuf_lock, flags);

	wake_up_interruptible(&port->waitqueue);

	if (is_console_port(port) && hvc_poll(port->cons.hvc))
		hvc_kick();
}
