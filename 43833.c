static int tdp_page_fault(struct kvm_vcpu *vcpu, gva_t gpa, u32 error_code,
			  bool prefault)
{
	pfn_t pfn;
	int r;
	int level;
	int force_pt_level;
	gfn_t gfn = gpa >> PAGE_SHIFT;
	unsigned long mmu_seq;
	int write = error_code & PFERR_WRITE_MASK;
	bool map_writable;

	ASSERT(vcpu);
	ASSERT(VALID_PAGE(vcpu->arch.mmu.root_hpa));

	if (unlikely(error_code & PFERR_RSVD_MASK)) {
		r = handle_mmio_page_fault(vcpu, gpa, error_code, true);

		if (likely(r != RET_MMIO_PF_INVALID))
			return r;
	}

	r = mmu_topup_memory_caches(vcpu);
	if (r)
		return r;

	force_pt_level = mapping_level_dirty_bitmap(vcpu, gfn);
	if (likely(!force_pt_level)) {
		level = mapping_level(vcpu, gfn);
		gfn &= ~(KVM_PAGES_PER_HPAGE(level) - 1);
	} else
		level = PT_PAGE_TABLE_LEVEL;

	if (fast_page_fault(vcpu, gpa, level, error_code))
		return 0;

	mmu_seq = vcpu->kvm->mmu_notifier_seq;
	smp_rmb();

	if (try_async_pf(vcpu, prefault, gfn, gpa, &pfn, write, &map_writable))
		return 0;

	if (handle_abnormal_pfn(vcpu, 0, gfn, pfn, ACC_ALL, &r))
		return r;

	spin_lock(&vcpu->kvm->mmu_lock);
	if (mmu_notifier_retry(vcpu->kvm, mmu_seq))
		goto out_unlock;
	make_mmu_pages_available(vcpu);
	if (likely(!force_pt_level))
		transparent_hugepage_adjust(vcpu, &gfn, &pfn, &level);
	r = __direct_map(vcpu, gpa, write, map_writable,
			 level, gfn, pfn, prefault);
	spin_unlock(&vcpu->kvm->mmu_lock);

	return r;

out_unlock:
	spin_unlock(&vcpu->kvm->mmu_lock);
	kvm_release_pfn_clean(pfn);
	return 0;
}
