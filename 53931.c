void kvm_define_shared_msr(unsigned slot, u32 msr)
{
	BUG_ON(slot >= KVM_NR_SHARED_MSRS);
	shared_msrs_global.msrs[slot] = msr;
	if (slot >= shared_msrs_global.nr)
		shared_msrs_global.nr = slot + 1;
}
