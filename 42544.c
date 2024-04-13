static void kvm_assigned_dev_ack_irq(struct kvm_irq_ack_notifier *kian)
{
	struct kvm_assigned_dev_kernel *dev;

	if (kian->gsi == -1)
		return;

	dev = container_of(kian, struct kvm_assigned_dev_kernel,
			   ack_notifier);

	kvm_set_irq(dev->kvm, dev->irq_source_id, dev->guest_irq, 0);

	/* The guest irq may be shared so this ack may be
	 * from another device.
	 */
	spin_lock(&dev->intx_lock);
	if (dev->host_irq_disabled) {
		enable_irq(dev->host_irq);
		dev->host_irq_disabled = false;
	}
	spin_unlock(&dev->intx_lock);
}
