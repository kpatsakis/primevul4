static int nonpaging_page_fault(struct kvm_vcpu *vcpu, gva_t gva,
				u32 error_code, bool prefault)
{
	gfn_t gfn;
	int r;

	pgprintk("%s: gva %lx error %x\n", __func__, gva, error_code);

	if (unlikely(error_code & PFERR_RSVD_MASK)) {
		r = handle_mmio_page_fault(vcpu, gva, error_code, true);

		if (likely(r != RET_MMIO_PF_INVALID))
			return r;
	}

	r = mmu_topup_memory_caches(vcpu);
	if (r)
		return r;

	ASSERT(vcpu);
	ASSERT(VALID_PAGE(vcpu->arch.mmu.root_hpa));

	gfn = gva >> PAGE_SHIFT;

	return nonpaging_map(vcpu, gva & PAGE_MASK,
			     error_code, gfn, prefault);
}
