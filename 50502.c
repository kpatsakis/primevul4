static int vfio_pci_set_msi_trigger(struct vfio_pci_device *vdev,
				    unsigned index, unsigned start,
				    unsigned count, uint32_t flags, void *data)
{
	int i;
	bool msix = (index == VFIO_PCI_MSIX_IRQ_INDEX) ? true : false;

	if (irq_is(vdev, index) && !count && (flags & VFIO_IRQ_SET_DATA_NONE)) {
		vfio_msi_disable(vdev, msix);
		return 0;
	}

	if (!(irq_is(vdev, index) || is_irq_none(vdev)))
		return -EINVAL;

	if (flags & VFIO_IRQ_SET_DATA_EVENTFD) {
		int32_t *fds = data;
		int ret;

		if (vdev->irq_type == index)
			return vfio_msi_set_block(vdev, start, count,
						  fds, msix);

		ret = vfio_msi_enable(vdev, start + count, msix);
		if (ret)
			return ret;

		ret = vfio_msi_set_block(vdev, start, count, fds, msix);
		if (ret)
			vfio_msi_disable(vdev, msix);

		return ret;
	}

	if (!irq_is(vdev, index) || start + count > vdev->num_ctx)
		return -EINVAL;

	for (i = start; i < start + count; i++) {
		if (!vdev->ctx[i].trigger)
			continue;
		if (flags & VFIO_IRQ_SET_DATA_NONE) {
			eventfd_signal(vdev->ctx[i].trigger, 1);
		} else if (flags & VFIO_IRQ_SET_DATA_BOOL) {
			uint8_t *bools = data;
			if (bools[i - start])
				eventfd_signal(vdev->ctx[i].trigger, 1);
		}
	}
	return 0;
}
