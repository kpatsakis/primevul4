static int assigned_device_enable_host_msix(struct kvm *kvm,
					    struct kvm_assigned_dev_kernel *dev)
{
	int i, r = -EINVAL;

	/* host_msix_entries and guest_msix_entries should have been
	 * initialized */
	if (dev->entries_nr == 0)
		return r;

	r = pci_enable_msix(dev->dev, dev->host_msix_entries, dev->entries_nr);
	if (r)
		return r;

	for (i = 0; i < dev->entries_nr; i++) {
		r = request_threaded_irq(dev->host_msix_entries[i].vector,
					 NULL, kvm_assigned_dev_thread,
					 0, dev->irq_name, (void *)dev);
		if (r)
			goto err;
	}

	return 0;
err:
	for (i -= 1; i >= 0; i--)
		free_irq(dev->host_msix_entries[i].vector, (void *)dev);
	pci_disable_msix(dev->dev);
	return r;
}
