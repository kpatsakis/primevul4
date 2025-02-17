static int vfio_msi_set_vector_signal(struct vfio_pci_device *vdev,
				      int vector, int fd, bool msix)
{
	struct pci_dev *pdev = vdev->pdev;
	struct eventfd_ctx *trigger;
	int irq, ret;

	if (vector < 0 || vector >= vdev->num_ctx)
		return -EINVAL;

	irq = pci_irq_vector(pdev, vector);

	if (vdev->ctx[vector].trigger) {
		free_irq(irq, vdev->ctx[vector].trigger);
		irq_bypass_unregister_producer(&vdev->ctx[vector].producer);
		kfree(vdev->ctx[vector].name);
		eventfd_ctx_put(vdev->ctx[vector].trigger);
		vdev->ctx[vector].trigger = NULL;
	}

	if (fd < 0)
		return 0;

	vdev->ctx[vector].name = kasprintf(GFP_KERNEL, "vfio-msi%s[%d](%s)",
					   msix ? "x" : "", vector,
					   pci_name(pdev));
	if (!vdev->ctx[vector].name)
		return -ENOMEM;

	trigger = eventfd_ctx_fdget(fd);
	if (IS_ERR(trigger)) {
		kfree(vdev->ctx[vector].name);
		return PTR_ERR(trigger);
	}

	/*
	 * The MSIx vector table resides in device memory which may be cleared
	 * via backdoor resets. We don't allow direct access to the vector
	 * table so even if a userspace driver attempts to save/restore around
	 * such a reset it would be unsuccessful. To avoid this, restore the
	 * cached value of the message prior to enabling.
	 */
	if (msix) {
		struct msi_msg msg;

		get_cached_msi_msg(irq, &msg);
		pci_write_msi_msg(irq, &msg);
	}

	ret = request_irq(irq, vfio_msihandler, 0,
			  vdev->ctx[vector].name, trigger);
	if (ret) {
		kfree(vdev->ctx[vector].name);
		eventfd_ctx_put(trigger);
		return ret;
	}

	vdev->ctx[vector].producer.token = trigger;
	vdev->ctx[vector].producer.irq = irq;
	ret = irq_bypass_register_producer(&vdev->ctx[vector].producer);
	if (unlikely(ret))
		dev_info(&pdev->dev,
		"irq bypass producer (token %p) registration fails: %d\n",
		vdev->ctx[vector].producer.token, ret);

	vdev->ctx[vector].trigger = trigger;

	return 0;
}
