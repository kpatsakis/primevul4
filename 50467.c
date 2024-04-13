static int vfio_pci_get_devs(struct pci_dev *pdev, void *data)
{
	struct vfio_devices *devs = data;
	struct vfio_device *device;

	if (devs->cur_index == devs->max_index)
		return -ENOSPC;

	device = vfio_device_get_from_dev(&pdev->dev);
	if (!device)
		return -EINVAL;

	if (pci_dev_driver(pdev) != &vfio_pci_driver) {
		vfio_device_put(device);
		return -EBUSY;
	}

	devs->devices[devs->cur_index++] = device;
	return 0;
}
