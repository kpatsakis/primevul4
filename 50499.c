static int vfio_pci_set_intx_mask(struct vfio_pci_device *vdev,
				  unsigned index, unsigned start,
				  unsigned count, uint32_t flags, void *data)
{
	if (!is_intx(vdev) || start != 0 || count != 1)
		return -EINVAL;

	if (flags & VFIO_IRQ_SET_DATA_NONE) {
		vfio_pci_intx_mask(vdev);
	} else if (flags & VFIO_IRQ_SET_DATA_BOOL) {
		uint8_t mask = *(uint8_t *)data;
		if (mask)
			vfio_pci_intx_mask(vdev);
	} else if (flags & VFIO_IRQ_SET_DATA_EVENTFD) {
		return -ENOTTY; /* XXX implement me */
	}

	return 0;
}
