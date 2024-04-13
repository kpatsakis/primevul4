static void vfio_pci_request(void *device_data, unsigned int count)
{
	struct vfio_pci_device *vdev = device_data;

	mutex_lock(&vdev->igate);

	if (vdev->req_trigger) {
		if (!(count % 10))
			dev_notice_ratelimited(&vdev->pdev->dev,
				"Relaying device request to user (#%u)\n",
				count);
		eventfd_signal(vdev->req_trigger, 1);
	} else if (count == 0) {
		dev_warn(&vdev->pdev->dev,
			"No device request channel registered, blocked until released by user\n");
	}

	mutex_unlock(&vdev->igate);
}
