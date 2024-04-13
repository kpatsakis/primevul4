static int vfio_pci_set_intx_trigger(struct vfio_pci_device *vdev,
				     unsigned index, unsigned start,
				     unsigned count, uint32_t flags, void *data)
{
	if (is_intx(vdev) && !count && (flags & VFIO_IRQ_SET_DATA_NONE)) {
		vfio_intx_disable(vdev);
		return 0;
	}

	if (!(is_intx(vdev) || is_irq_none(vdev)) || start != 0 || count != 1)
		return -EINVAL;

	if (flags & VFIO_IRQ_SET_DATA_EVENTFD) {
		int32_t fd = *(int32_t *)data;
		int ret;

		if (is_intx(vdev))
			return vfio_intx_set_signal(vdev, fd);

		ret = vfio_intx_enable(vdev);
		if (ret)
			return ret;

		ret = vfio_intx_set_signal(vdev, fd);
		if (ret)
			vfio_intx_disable(vdev);

		return ret;
	}

	if (!is_intx(vdev))
		return -EINVAL;

	if (flags & VFIO_IRQ_SET_DATA_NONE) {
		vfio_send_intx_eventfd(vdev, NULL);
	} else if (flags & VFIO_IRQ_SET_DATA_BOOL) {
		uint8_t trigger = *(uint8_t *)data;
		if (trigger)
			vfio_send_intx_eventfd(vdev, NULL);
	}
	return 0;
}
