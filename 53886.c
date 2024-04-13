static int do_get_msr(struct kvm_vcpu *vcpu, unsigned index, u64 *data)
{
	struct msr_data msr;
	int r;

	msr.index = index;
	msr.host_initiated = true;
	r = kvm_get_msr(vcpu, &msr);
	if (r)
		return r;

	*data = msr.data;
	return 0;
}
