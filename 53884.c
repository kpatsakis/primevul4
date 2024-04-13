static inline void adjust_tsc_offset_host(struct kvm_vcpu *vcpu, s64 adjustment)
{
	if (vcpu->arch.tsc_scaling_ratio != kvm_default_tsc_scaling_ratio)
		WARN_ON(adjustment < 0);
	adjustment = kvm_scale_tsc(vcpu, (u64) adjustment);
	kvm_x86_ops->adjust_tsc_offset_guest(vcpu, adjustment);
}
