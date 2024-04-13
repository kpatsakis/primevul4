static int handle_mmio_page_fault(struct kvm_vcpu *vcpu, u64 addr,
				  u32 error_code, bool direct)
{
	int ret;

	ret = handle_mmio_page_fault_common(vcpu, addr, direct);
	WARN_ON(ret == RET_MMIO_PF_BUG);
	return ret;
}
