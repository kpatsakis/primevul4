static int kvm_vm_ioctl_assign_irq(struct kvm *kvm,
				   struct kvm_assigned_irq *assigned_irq)
{
	int r = -EINVAL;
	struct kvm_assigned_dev_kernel *match;
	unsigned long host_irq_type, guest_irq_type;

	if (!irqchip_in_kernel(kvm))
		return r;

	mutex_lock(&kvm->lock);
	r = -ENODEV;
	match = kvm_find_assigned_dev(&kvm->arch.assigned_dev_head,
				      assigned_irq->assigned_dev_id);
	if (!match)
		goto out;

	host_irq_type = (assigned_irq->flags & KVM_DEV_IRQ_HOST_MASK);
	guest_irq_type = (assigned_irq->flags & KVM_DEV_IRQ_GUEST_MASK);

	r = -EINVAL;
	/* can only assign one type at a time */
	if (hweight_long(host_irq_type) > 1)
		goto out;
	if (hweight_long(guest_irq_type) > 1)
		goto out;
	if (host_irq_type == 0 && guest_irq_type == 0)
		goto out;

	r = 0;
	if (host_irq_type)
		r = assign_host_irq(kvm, match, host_irq_type);
	if (r)
		goto out;

	if (guest_irq_type)
		r = assign_guest_irq(kvm, match, assigned_irq, guest_irq_type);
out:
	mutex_unlock(&kvm->lock);
	return r;
}
