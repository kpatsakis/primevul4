static unsigned long emulator_get_cached_segment_base(int seg,
						      struct kvm_vcpu *vcpu)
{
	return get_segment_base(vcpu, seg);
}
