static void vfio_intx_disable(struct vfio_pci_device *vdev)
{
	vfio_virqfd_disable(&vdev->ctx[0].unmask);
	vfio_virqfd_disable(&vdev->ctx[0].mask);
	vfio_intx_set_signal(vdev, -1);
	vdev->irq_type = VFIO_PCI_NUM_IRQS;
	vdev->num_ctx = 0;
	kfree(vdev->ctx);
}
