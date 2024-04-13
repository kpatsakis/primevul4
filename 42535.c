static int assigned_device_enable_guest_intx(struct kvm *kvm,
				struct kvm_assigned_dev_kernel *dev,
				struct kvm_assigned_irq *irq)
{
	dev->guest_irq = irq->guest_irq;
	dev->ack_notifier.gsi = irq->guest_irq;
	return 0;
}
