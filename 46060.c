int kvm_arch_vcpu_runnable(struct kvm_vcpu *vcpu)
{
	return (vcpu->arch.mp_state == KVM_MP_STATE_RUNNABLE &&
		!vcpu->arch.apf.halted)
		|| !list_empty_careful(&vcpu->async_pf.done)
		|| vcpu->arch.mp_state == KVM_MP_STATE_SIPI_RECEIVED
		|| vcpu->arch.nmi_pending ||
		(kvm_arch_interrupt_allowed(vcpu) &&
		 kvm_cpu_has_interrupt(vcpu));
}
