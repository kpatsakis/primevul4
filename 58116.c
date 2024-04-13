static int virtcons_restore(struct virtio_device *vdev)
{
	struct ports_device *portdev;
	struct port *port;
	int ret;

	portdev = vdev->priv;

	ret = init_vqs(portdev);
	if (ret)
		return ret;

	virtio_device_ready(portdev->vdev);

	if (use_multiport(portdev))
		fill_queue(portdev->c_ivq, &portdev->c_ivq_lock);

	list_for_each_entry(port, &portdev->ports, list) {
		port->in_vq = portdev->in_vqs[port->id];
		port->out_vq = portdev->out_vqs[port->id];

		fill_queue(port->in_vq, &port->inbuf_lock);

		/* Get port open/close status on the host */
		send_control_msg(port, VIRTIO_CONSOLE_PORT_READY, 1);

		/*
		 * If a port was open at the time of suspending, we
		 * have to let the host know that it's still open.
		 */
		if (port->guest_connected)
			send_control_msg(port, VIRTIO_CONSOLE_PORT_OPEN, 1);
	}
	return 0;
}
