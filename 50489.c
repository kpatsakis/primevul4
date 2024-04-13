static int vfio_intx_set_signal(struct vfio_pci_device *vdev, int fd)
{
	struct pci_dev *pdev = vdev->pdev;
	unsigned long irqflags = IRQF_SHARED;
	struct eventfd_ctx *trigger;
	unsigned long flags;
	int ret;

	if (vdev->ctx[0].trigger) {
		free_irq(pdev->irq, vdev);
		kfree(vdev->ctx[0].name);
		eventfd_ctx_put(vdev->ctx[0].trigger);
		vdev->ctx[0].trigger = NULL;
	}

	if (fd < 0) /* Disable only */
		return 0;

	vdev->ctx[0].name = kasprintf(GFP_KERNEL, "vfio-intx(%s)",
				      pci_name(pdev));
	if (!vdev->ctx[0].name)
		return -ENOMEM;

	trigger = eventfd_ctx_fdget(fd);
	if (IS_ERR(trigger)) {
		kfree(vdev->ctx[0].name);
		return PTR_ERR(trigger);
	}

	vdev->ctx[0].trigger = trigger;

	if (!vdev->pci_2_3)
		irqflags = 0;

	ret = request_irq(pdev->irq, vfio_intx_handler,
			  irqflags, vdev->ctx[0].name, vdev);
	if (ret) {
		vdev->ctx[0].trigger = NULL;
		kfree(vdev->ctx[0].name);
		eventfd_ctx_put(trigger);
		return ret;
	}

	/*
	 * INTx disable will stick across the new irq setup,
	 * disable_irq won't.
	 */
	spin_lock_irqsave(&vdev->irqlock, flags);
	if (!vdev->pci_2_3 && vdev->ctx[0].masked)
		disable_irq_nosync(pdev->irq);
	spin_unlock_irqrestore(&vdev->irqlock, flags);

	return 0;
}
