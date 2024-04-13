static void config_intr(struct virtio_device *vdev)
{
	struct ports_device *portdev;

	portdev = vdev->priv;

	if (!use_multiport(portdev))
		schedule_work(&portdev->config_work);
}
