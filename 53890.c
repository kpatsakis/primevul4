static int emulator_get_msr(struct x86_emulate_ctxt *ctxt,
			    u32 msr_index, u64 *pdata)
{
	struct msr_data msr;
	int r;

	msr.index = msr_index;
	msr.host_initiated = false;
	r = kvm_get_msr(emul_to_vcpu(ctxt), &msr);
	if (r)
		return r;

	*pdata = msr.data;
	return 0;
}
