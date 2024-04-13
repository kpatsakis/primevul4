static void kvm_free_assigned_irq(struct kvm *kvm,
				  struct kvm_assigned_dev_kernel *assigned_dev)
{
	kvm_deassign_irq(kvm, assigned_dev, assigned_dev->irq_requested_type);
}
