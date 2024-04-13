static bool can_do_async_pf(struct kvm_vcpu *vcpu)
{
	if (unlikely(!irqchip_in_kernel(vcpu->kvm) ||
		     kvm_event_needs_reinjection(vcpu)))
		return false;

	return kvm_x86_ops->interrupt_allowed(vcpu);
}
