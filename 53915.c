int kvm_arch_update_irqfd_routing(struct kvm *kvm, unsigned int host_irq,
				   uint32_t guest_irq, bool set)
{
	if (!kvm_x86_ops->update_pi_irte)
		return -EINVAL;

	return kvm_x86_ops->update_pi_irte(kvm, host_irq, guest_irq, set);
}
