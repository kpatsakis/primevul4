static void virtcons_remove(struct virtio_device *vdev)
{
	struct ports_device *portdev;
	struct port *port, *port2;

	portdev = vdev->priv;

	spin_lock_irq(&pdrvdata_lock);
	list_del(&portdev->list);
	spin_unlock_irq(&pdrvdata_lock);

	/* Disable interrupts for vqs */
	vdev->config->reset(vdev);
	/* Finish up work that's lined up */
	if (use_multiport(portdev))
		cancel_work_sync(&portdev->control_work);
	else
		cancel_work_sync(&portdev->config_work);

	list_for_each_entry_safe(port, port2, &portdev->ports, list)
		unplug_port(port);

	unregister_chrdev(portdev->chr_major, "virtio-portsdev");

	/*
	 * When yanking out a device, we immediately lose the
	 * (device-side) queues.  So there's no point in keeping the
	 * guest side around till we drop our final reference.  This
	 * also means that any ports which are in an open state will
	 * have to just stop using the port, as the vqs are going
	 * away.
	 */
	remove_controlq_data(portdev);
	remove_vqs(portdev);
	kfree(portdev);
}
