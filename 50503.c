static int vfio_pci_set_req_trigger(struct vfio_pci_device *vdev,
				    unsigned index, unsigned start,
				    unsigned count, uint32_t flags, void *data)
{
	if (index != VFIO_PCI_REQ_IRQ_INDEX || start != 0 || count > 1)
		return -EINVAL;

	return vfio_pci_set_ctx_trigger_single(&vdev->req_trigger,
					       count, flags, data);
}
