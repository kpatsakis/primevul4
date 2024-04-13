static int add_port(struct ports_device *portdev, u32 id)
{
	char debugfs_name[16];
	struct port *port;
	struct port_buffer *buf;
	dev_t devt;
	unsigned int nr_added_bufs;
	int err;

	port = kmalloc(sizeof(*port), GFP_KERNEL);
	if (!port) {
		err = -ENOMEM;
		goto fail;
	}
	kref_init(&port->kref);

	port->portdev = portdev;
	port->id = id;

	port->name = NULL;
	port->inbuf = NULL;
	port->cons.hvc = NULL;
	port->async_queue = NULL;

	port->cons.ws.ws_row = port->cons.ws.ws_col = 0;

	port->host_connected = port->guest_connected = false;
	port->stats = (struct port_stats) { 0 };

	port->outvq_full = false;

	port->in_vq = portdev->in_vqs[port->id];
	port->out_vq = portdev->out_vqs[port->id];

	port->cdev = cdev_alloc();
	if (!port->cdev) {
		dev_err(&port->portdev->vdev->dev, "Error allocating cdev\n");
		err = -ENOMEM;
		goto free_port;
	}
	port->cdev->ops = &port_fops;

	devt = MKDEV(portdev->chr_major, id);
	err = cdev_add(port->cdev, devt, 1);
	if (err < 0) {
		dev_err(&port->portdev->vdev->dev,
			"Error %d adding cdev for port %u\n", err, id);
		goto free_cdev;
	}
	port->dev = device_create(pdrvdata.class, &port->portdev->vdev->dev,
				  devt, port, "vport%up%u",
				  port->portdev->vdev->index, id);
	if (IS_ERR(port->dev)) {
		err = PTR_ERR(port->dev);
		dev_err(&port->portdev->vdev->dev,
			"Error %d creating device for port %u\n",
			err, id);
		goto free_cdev;
	}

	spin_lock_init(&port->inbuf_lock);
	spin_lock_init(&port->outvq_lock);
	init_waitqueue_head(&port->waitqueue);

	/* Fill the in_vq with buffers so the host can send us data. */
	nr_added_bufs = fill_queue(port->in_vq, &port->inbuf_lock);
	if (!nr_added_bufs) {
		dev_err(port->dev, "Error allocating inbufs\n");
		err = -ENOMEM;
		goto free_device;
	}

	if (is_rproc_serial(port->portdev->vdev))
		/*
		 * For rproc_serial assume remote processor is connected.
		 * rproc_serial does not want the console port, only
		 * the generic port implementation.
		 */
		port->host_connected = true;
	else if (!use_multiport(port->portdev)) {
		/*
		 * If we're not using multiport support,
		 * this has to be a console port.
		 */
		err = init_port_console(port);
		if (err)
			goto free_inbufs;
	}

	spin_lock_irq(&portdev->ports_lock);
	list_add_tail(&port->list, &port->portdev->ports);
	spin_unlock_irq(&portdev->ports_lock);

	/*
	 * Tell the Host we're set so that it can send us various
	 * configuration parameters for this port (eg, port name,
	 * caching, whether this is a console port, etc.)
	 */
	send_control_msg(port, VIRTIO_CONSOLE_PORT_READY, 1);

	if (pdrvdata.debugfs_dir) {
		/*
		 * Finally, create the debugfs file that we can use to
		 * inspect a port's state at any time
		 */
		snprintf(debugfs_name, sizeof(debugfs_name), "vport%up%u",
			 port->portdev->vdev->index, id);
		port->debugfs_file = debugfs_create_file(debugfs_name, 0444,
							 pdrvdata.debugfs_dir,
							 port,
							 &port_debugfs_ops);
	}
	return 0;

free_inbufs:
	while ((buf = virtqueue_detach_unused_buf(port->in_vq)))
		free_buf(buf, true);
free_device:
	device_destroy(pdrvdata.class, port->dev->devt);
free_cdev:
	cdev_del(port->cdev);
free_port:
	kfree(port);
fail:
	/* The host might want to notify management sw about port add failure */
	__send_control_msg(portdev, id, VIRTIO_CONSOLE_PORT_READY, 0);
	return err;
}
