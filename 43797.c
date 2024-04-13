static int nonpaging_map(struct kvm_vcpu *vcpu, gva_t v, u32 error_code,
			 gfn_t gfn, bool prefault)
{
	int r;
	int level;
	int force_pt_level;
	pfn_t pfn;
	unsigned long mmu_seq;
	bool map_writable, write = error_code & PFERR_WRITE_MASK;

	force_pt_level = mapping_level_dirty_bitmap(vcpu, gfn);
	if (likely(!force_pt_level)) {
		level = mapping_level(vcpu, gfn);
		/*
		 * This path builds a PAE pagetable - so we can map
		 * 2mb pages at maximum. Therefore check if the level
		 * is larger than that.
		 */
		if (level > PT_DIRECTORY_LEVEL)
			level = PT_DIRECTORY_LEVEL;

		gfn &= ~(KVM_PAGES_PER_HPAGE(level) - 1);
	} else
		level = PT_PAGE_TABLE_LEVEL;

	if (fast_page_fault(vcpu, v, level, error_code))
		return 0;

	mmu_seq = vcpu->kvm->mmu_notifier_seq;
	smp_rmb();

	if (try_async_pf(vcpu, prefault, gfn, v, &pfn, write, &map_writable))
		return 0;

	if (handle_abnormal_pfn(vcpu, v, gfn, pfn, ACC_ALL, &r))
		return r;

	spin_lock(&vcpu->kvm->mmu_lock);
	if (mmu_notifier_retry(vcpu->kvm, mmu_seq))
		goto out_unlock;
	make_mmu_pages_available(vcpu);
	if (likely(!force_pt_level))
		transparent_hugepage_adjust(vcpu, &gfn, &pfn, &level);
	r = __direct_map(vcpu, v, write, map_writable, level, gfn, pfn,
			 prefault);
	spin_unlock(&vcpu->kvm->mmu_lock);


	return r;

out_unlock:
	spin_unlock(&vcpu->kvm->mmu_lock);
	kvm_release_pfn_clean(pfn);
	return 0;
}
