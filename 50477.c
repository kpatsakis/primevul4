static void vfio_pci_release(void *device_data)
{
	struct vfio_pci_device *vdev = device_data;

	mutex_lock(&driver_lock);

	if (!(--vdev->refcnt)) {
		vfio_spapr_pci_eeh_release(vdev->pdev);
		vfio_pci_disable(vdev);
	}

	mutex_unlock(&driver_lock);

	module_put(THIS_MODULE);
}
