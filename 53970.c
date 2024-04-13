static inline bool kvm_vcpu_has_events(struct kvm_vcpu *vcpu)
{
	if (!list_empty_careful(&vcpu->async_pf.done))
		return true;

	if (kvm_apic_has_events(vcpu))
		return true;

	if (vcpu->arch.pv.pv_unhalted)
		return true;

	if (atomic_read(&vcpu->arch.nmi_queued))
		return true;

	if (test_bit(KVM_REQ_SMI, &vcpu->requests))
		return true;

	if (kvm_arch_interrupt_allowed(vcpu) &&
	    kvm_cpu_has_interrupt(vcpu))
		return true;

	return false;
}
