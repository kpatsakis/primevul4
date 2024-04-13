static int vfio_pci_set_err_trigger(struct vfio_pci_device *vdev,
				    unsigned index, unsigned start,
				    unsigned count, uint32_t flags, void *data)
{
	if (index != VFIO_PCI_ERR_IRQ_INDEX || start != 0 || count > 1)
		return -EINVAL;

	return vfio_pci_set_ctx_trigger_single(&vdev->err_trigger,
					       count, flags, data);
}
