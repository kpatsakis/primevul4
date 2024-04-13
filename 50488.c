static irqreturn_t vfio_intx_handler(int irq, void *dev_id)
{
	struct vfio_pci_device *vdev = dev_id;
	unsigned long flags;
	int ret = IRQ_NONE;

	spin_lock_irqsave(&vdev->irqlock, flags);

	if (!vdev->pci_2_3) {
		disable_irq_nosync(vdev->pdev->irq);
		vdev->ctx[0].masked = true;
		ret = IRQ_HANDLED;
	} else if (!vdev->ctx[0].masked &&  /* may be shared */
		   pci_check_and_mask_intx(vdev->pdev)) {
		vdev->ctx[0].masked = true;
		ret = IRQ_HANDLED;
	}

	spin_unlock_irqrestore(&vdev->irqlock, flags);

	if (ret == IRQ_HANDLED)
		vfio_send_intx_eventfd(vdev, NULL);

	return ret;
}
