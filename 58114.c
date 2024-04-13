static int virtcons_probe(struct virtio_device *vdev)
{
	struct ports_device *portdev;
	int err;
	bool multiport;
	bool early = early_put_chars != NULL;

	/* We only need a config space if features are offered */
	if (!vdev->config->get &&
	    (virtio_has_feature(vdev, VIRTIO_CONSOLE_F_SIZE)
	     || virtio_has_feature(vdev, VIRTIO_CONSOLE_F_MULTIPORT))) {
		dev_err(&vdev->dev, "%s failure: config access disabled\n",
			__func__);
		return -EINVAL;
	}

	/* Ensure to read early_put_chars now */
	barrier();

	portdev = kmalloc(sizeof(*portdev), GFP_KERNEL);
	if (!portdev) {
		err = -ENOMEM;
		goto fail;
	}

	/* Attach this portdev to this virtio_device, and vice-versa. */
	portdev->vdev = vdev;
	vdev->priv = portdev;

	portdev->chr_major = register_chrdev(0, "virtio-portsdev",
					     &portdev_fops);
	if (portdev->chr_major < 0) {
		dev_err(&vdev->dev,
			"Error %d registering chrdev for device %u\n",
			portdev->chr_major, vdev->index);
		err = portdev->chr_major;
		goto free;
	}

	multiport = false;
	portdev->max_nr_ports = 1;

	/* Don't test MULTIPORT at all if we're rproc: not a valid feature! */
	if (!is_rproc_serial(vdev) &&
	    virtio_cread_feature(vdev, VIRTIO_CONSOLE_F_MULTIPORT,
				 struct virtio_console_config, max_nr_ports,
				 &portdev->max_nr_ports) == 0) {
		multiport = true;
	}

	err = init_vqs(portdev);
	if (err < 0) {
		dev_err(&vdev->dev, "Error %d initializing vqs\n", err);
		goto free_chrdev;
	}

	spin_lock_init(&portdev->ports_lock);
	INIT_LIST_HEAD(&portdev->ports);

	virtio_device_ready(portdev->vdev);

	INIT_WORK(&portdev->config_work, &config_work_handler);
	INIT_WORK(&portdev->control_work, &control_work_handler);

	if (multiport) {
		unsigned int nr_added_bufs;

		spin_lock_init(&portdev->c_ivq_lock);
		spin_lock_init(&portdev->c_ovq_lock);

		nr_added_bufs = fill_queue(portdev->c_ivq,
					   &portdev->c_ivq_lock);
		if (!nr_added_bufs) {
			dev_err(&vdev->dev,
				"Error allocating buffers for control queue\n");
			err = -ENOMEM;
			goto free_vqs;
		}
	} else {
		/*
		 * For backward compatibility: Create a console port
		 * if we're running on older host.
		 */
		add_port(portdev, 0);
	}

	spin_lock_irq(&pdrvdata_lock);
	list_add_tail(&portdev->list, &pdrvdata.portdevs);
	spin_unlock_irq(&pdrvdata_lock);

	__send_control_msg(portdev, VIRTIO_CONSOLE_BAD_ID,
			   VIRTIO_CONSOLE_DEVICE_READY, 1);

	/*
	 * If there was an early virtio console, assume that there are no
	 * other consoles. We need to wait until the hvc_alloc matches the
	 * hvc_instantiate, otherwise tty_open will complain, resulting in
	 * a "Warning: unable to open an initial console" boot failure.
	 * Without multiport this is done in add_port above. With multiport
	 * this might take some host<->guest communication - thus we have to
	 * wait.
	 */
	if (multiport && early)
		wait_for_completion(&early_console_added);

	return 0;

free_vqs:
	/* The host might want to notify mgmt sw about device add failure */
	__send_control_msg(portdev, VIRTIO_CONSOLE_BAD_ID,
			   VIRTIO_CONSOLE_DEVICE_READY, 0);
	remove_vqs(portdev);
free_chrdev:
	unregister_chrdev(portdev->chr_major, "virtio-portsdev");
free:
	kfree(portdev);
fail:
	return err;
}
