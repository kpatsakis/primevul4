void vfio_pci_intx_mask(struct vfio_pci_device *vdev)
{
	struct pci_dev *pdev = vdev->pdev;
	unsigned long flags;

	spin_lock_irqsave(&vdev->irqlock, flags);

	/*
	 * Masking can come from interrupt, ioctl, or config space
	 * via INTx disable.  The latter means this can get called
	 * even when not using intx delivery.  In this case, just
	 * try to have the physical bit follow the virtual bit.
	 */
	if (unlikely(!is_intx(vdev))) {
		if (vdev->pci_2_3)
			pci_intx(pdev, 0);
	} else if (!vdev->ctx[0].masked) {
		/*
		 * Can't use check_and_mask here because we always want to
		 * mask, not just when something is pending.
		 */
		if (vdev->pci_2_3)
			pci_intx(pdev, 0);
		else
			disable_irq_nosync(pdev->irq);

		vdev->ctx[0].masked = true;
	}

	spin_unlock_irqrestore(&vdev->irqlock, flags);
}
