static void config_work_handler(struct work_struct *work)
{
	struct ports_device *portdev;

	portdev = container_of(work, struct ports_device, config_work);
	if (!use_multiport(portdev)) {
		struct virtio_device *vdev;
		struct port *port;
		u16 rows, cols;

		vdev = portdev->vdev;
		virtio_cread(vdev, struct virtio_console_config, cols, &cols);
		virtio_cread(vdev, struct virtio_console_config, rows, &rows);

		port = find_port_by_id(portdev, 0);
		set_console_size(port, rows, cols);

		/*
		 * We'll use this way of resizing only for legacy
		 * support.  For newer userspace
		 * (VIRTIO_CONSOLE_F_MULTPORT+), use control messages
		 * to indicate console size changes so that it can be
		 * done per-port.
		 */
		resize_console(port);
	}
}
