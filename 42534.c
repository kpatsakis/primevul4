static int assign_host_irq(struct kvm *kvm,
			   struct kvm_assigned_dev_kernel *dev,
			   __u32 host_irq_type)
{
	int r = -EEXIST;

	if (dev->irq_requested_type & KVM_DEV_IRQ_HOST_MASK)
		return r;

	snprintf(dev->irq_name, sizeof(dev->irq_name), "kvm:%s",
		 pci_name(dev->dev));

	switch (host_irq_type) {
	case KVM_DEV_IRQ_HOST_INTX:
		r = assigned_device_enable_host_intx(kvm, dev);
		break;
#ifdef __KVM_HAVE_MSI
	case KVM_DEV_IRQ_HOST_MSI:
		r = assigned_device_enable_host_msi(kvm, dev);
		break;
#endif
#ifdef __KVM_HAVE_MSIX
	case KVM_DEV_IRQ_HOST_MSIX:
		r = assigned_device_enable_host_msix(kvm, dev);
		break;
#endif
	default:
		r = -EINVAL;
	}

	if (!r)
		dev->irq_requested_type |= host_irq_type;

	return r;
}
