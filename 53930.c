static u64 kvm_compute_tsc_offset(struct kvm_vcpu *vcpu, u64 target_tsc)
{
	u64 tsc;

	tsc = kvm_scale_tsc(vcpu, rdtsc());

	return target_tsc - tsc;
}
