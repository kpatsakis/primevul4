static inline bool nested_vmx_allowed(struct kvm_vcpu *vcpu)
{
	return nested && guest_cpuid_has(vcpu, X86_FEATURE_VMX);
}
