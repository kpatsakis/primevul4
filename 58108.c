static void resize_console(struct port *port)
{
	struct virtio_device *vdev;

	/* The port could have been hot-unplugged */
	if (!port || !is_console_port(port))
		return;

	vdev = port->portdev->vdev;

	/* Don't test F_SIZE at all if we're rproc: not a valid feature! */
	if (!is_rproc_serial(vdev) &&
	    virtio_has_feature(vdev, VIRTIO_CONSOLE_F_SIZE))
		hvc_resize(port->cons.hvc, port->cons.ws);
}
