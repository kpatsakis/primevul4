static u64 guest_read_tsc(struct kvm_vcpu *vcpu)
{
	u64 host_tsc, tsc_offset;

	host_tsc = rdtsc();
	tsc_offset = vmcs_read64(TSC_OFFSET);
	return kvm_scale_tsc(vcpu, host_tsc) + tsc_offset;
}
