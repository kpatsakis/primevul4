u64 kvm_scale_tsc(struct kvm_vcpu *vcpu, u64 tsc)
{
	u64 _tsc = tsc;
	u64 ratio = vcpu->arch.tsc_scaling_ratio;

	if (ratio != kvm_default_tsc_scaling_ratio)
		_tsc = __scale_tsc(ratio, tsc);

	return _tsc;
}
