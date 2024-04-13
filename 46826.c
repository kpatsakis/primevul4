static inline bool cpu_need_tpr_shadow(struct kvm_vcpu *vcpu)
{
	return cpu_has_vmx_tpr_shadow() && lapic_in_kernel(vcpu);
}
