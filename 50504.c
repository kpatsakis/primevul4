static void vfio_send_intx_eventfd(void *opaque, void *unused)
{
	struct vfio_pci_device *vdev = opaque;

	if (likely(is_intx(vdev) && !vdev->virq_disabled))
		eventfd_signal(vdev->ctx[0].trigger, 1);
}
