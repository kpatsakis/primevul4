static int assigned_device_enable_guest_msi(struct kvm *kvm,
			struct kvm_assigned_dev_kernel *dev,
			struct kvm_assigned_irq *irq)
{
	dev->guest_irq = irq->guest_irq;
	dev->ack_notifier.gsi = -1;
	dev->host_irq_disabled = false;
	return 0;
}
