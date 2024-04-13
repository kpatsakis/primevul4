static int assigned_device_enable_host_msi(struct kvm *kvm,
					   struct kvm_assigned_dev_kernel *dev)
{
	int r;

	if (!dev->dev->msi_enabled) {
		r = pci_enable_msi(dev->dev);
		if (r)
			return r;
	}

	dev->host_irq = dev->dev->irq;
	if (request_threaded_irq(dev->host_irq, NULL, kvm_assigned_dev_thread,
				 0, dev->irq_name, (void *)dev)) {
		pci_disable_msi(dev->dev);
		return -EIO;
	}

	return 0;
}
