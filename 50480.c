static ssize_t vfio_pci_rw(void *device_data, char __user *buf,
			   size_t count, loff_t *ppos, bool iswrite)
{
	unsigned int index = VFIO_PCI_OFFSET_TO_INDEX(*ppos);
	struct vfio_pci_device *vdev = device_data;

	if (index >= VFIO_PCI_NUM_REGIONS + vdev->num_regions)
		return -EINVAL;

	switch (index) {
	case VFIO_PCI_CONFIG_REGION_INDEX:
		return vfio_pci_config_rw(vdev, buf, count, ppos, iswrite);

	case VFIO_PCI_ROM_REGION_INDEX:
		if (iswrite)
			return -EINVAL;
		return vfio_pci_bar_rw(vdev, buf, count, ppos, false);

	case VFIO_PCI_BAR0_REGION_INDEX ... VFIO_PCI_BAR5_REGION_INDEX:
		return vfio_pci_bar_rw(vdev, buf, count, ppos, iswrite);

	case VFIO_PCI_VGA_REGION_INDEX:
		return vfio_pci_vga_rw(vdev, buf, count, ppos, iswrite);
	default:
		index -= VFIO_PCI_NUM_REGIONS;
		return vdev->region[index].ops->rw(vdev, buf,
						   count, ppos, iswrite);
	}

	return -EINVAL;
}
