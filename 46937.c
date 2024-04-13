static int vmx_set_tss_addr(struct kvm *kvm, unsigned int addr)
{
	int ret;

	ret = x86_set_memory_region(kvm, TSS_PRIVATE_MEMSLOT, addr,
				    PAGE_SIZE * 3);
	if (ret)
		return ret;
	kvm->arch.tss_addr = addr;
	return init_rmode_tss(kvm);
}
