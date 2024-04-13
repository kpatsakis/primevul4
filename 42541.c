static void deassign_guest_irq(struct kvm *kvm,
			       struct kvm_assigned_dev_kernel *assigned_dev)
{
	kvm_unregister_irq_ack_notifier(kvm, &assigned_dev->ack_notifier);
	assigned_dev->ack_notifier.gsi = -1;

	kvm_set_irq(assigned_dev->kvm, assigned_dev->irq_source_id,
		    assigned_dev->guest_irq, 0);

	if (assigned_dev->irq_source_id != -1)
		kvm_free_irq_source_id(kvm, assigned_dev->irq_source_id);
	assigned_dev->irq_source_id = -1;
	assigned_dev->irq_requested_type &= ~(KVM_DEV_IRQ_GUEST_MASK);
}
