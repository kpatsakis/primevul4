static int assigned_device_enable_host_intx(struct kvm *kvm,
					    struct kvm_assigned_dev_kernel *dev)
{
	dev->host_irq = dev->dev->irq;
	/* Even though this is PCI, we don't want to use shared
	 * interrupts. Sharing host devices with guest-assigned devices
	 * on the same interrupt line is not a happy situation: there
	 * are going to be long delays in accepting, acking, etc.
	 */
	if (request_threaded_irq(dev->host_irq, NULL, kvm_assigned_dev_thread,
				 IRQF_ONESHOT, dev->irq_name, (void *)dev))
		return -EIO;
	return 0;
}
