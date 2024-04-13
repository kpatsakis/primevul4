static void deassign_host_irq(struct kvm *kvm,
			      struct kvm_assigned_dev_kernel *assigned_dev)
{
	/*
	 * We disable irq here to prevent further events.
	 *
	 * Notice this maybe result in nested disable if the interrupt type is
	 * INTx, but it's OK for we are going to free it.
	 *
	 * If this function is a part of VM destroy, please ensure that till
	 * now, the kvm state is still legal for probably we also have to wait
	 * on a currently running IRQ handler.
	 */
	if (assigned_dev->irq_requested_type & KVM_DEV_IRQ_HOST_MSIX) {
		int i;
		for (i = 0; i < assigned_dev->entries_nr; i++)
			disable_irq(assigned_dev->host_msix_entries[i].vector);

		for (i = 0; i < assigned_dev->entries_nr; i++)
			free_irq(assigned_dev->host_msix_entries[i].vector,
				 (void *)assigned_dev);

		assigned_dev->entries_nr = 0;
		kfree(assigned_dev->host_msix_entries);
		kfree(assigned_dev->guest_msix_entries);
		pci_disable_msix(assigned_dev->dev);
	} else {
		/* Deal with MSI and INTx */
		disable_irq(assigned_dev->host_irq);

		free_irq(assigned_dev->host_irq, (void *)assigned_dev);

		if (assigned_dev->irq_requested_type & KVM_DEV_IRQ_HOST_MSI)
			pci_disable_msi(assigned_dev->dev);
	}

	assigned_dev->irq_requested_type &= ~(KVM_DEV_IRQ_HOST_MASK);
}
