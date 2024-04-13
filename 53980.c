static int kvm_vcpu_ready_for_interrupt_injection(struct kvm_vcpu *vcpu)
{
	return kvm_arch_interrupt_allowed(vcpu) &&
		!kvm_cpu_has_interrupt(vcpu) &&
		!kvm_event_needs_reinjection(vcpu) &&
		kvm_cpu_accept_dm_intr(vcpu);
}
