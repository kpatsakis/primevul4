static void mmu_set_spte(struct kvm_vcpu *vcpu, u64 *sptep,
			 unsigned pte_access, int write_fault, int *emulate,
			 int level, gfn_t gfn, pfn_t pfn, bool speculative,
			 bool host_writable)
{
	int was_rmapped = 0;
	int rmap_count;

	pgprintk("%s: spte %llx write_fault %d gfn %llx\n", __func__,
		 *sptep, write_fault, gfn);

	if (is_rmap_spte(*sptep)) {
		/*
		 * If we overwrite a PTE page pointer with a 2MB PMD, unlink
		 * the parent of the now unreachable PTE.
		 */
		if (level > PT_PAGE_TABLE_LEVEL &&
		    !is_large_pte(*sptep)) {
			struct kvm_mmu_page *child;
			u64 pte = *sptep;

			child = page_header(pte & PT64_BASE_ADDR_MASK);
			drop_parent_pte(child, sptep);
			kvm_flush_remote_tlbs(vcpu->kvm);
		} else if (pfn != spte_to_pfn(*sptep)) {
			pgprintk("hfn old %llx new %llx\n",
				 spte_to_pfn(*sptep), pfn);
			drop_spte(vcpu->kvm, sptep);
			kvm_flush_remote_tlbs(vcpu->kvm);
		} else
			was_rmapped = 1;
	}

	if (set_spte(vcpu, sptep, pte_access, level, gfn, pfn, speculative,
	      true, host_writable)) {
		if (write_fault)
			*emulate = 1;
		kvm_mmu_flush_tlb(vcpu);
	}

	if (unlikely(is_mmio_spte(*sptep) && emulate))
		*emulate = 1;

	pgprintk("%s: setting spte %llx\n", __func__, *sptep);
	pgprintk("instantiating %s PTE (%s) at %llx (%llx) addr %p\n",
		 is_large_pte(*sptep)? "2MB" : "4kB",
		 *sptep & PT_PRESENT_MASK ?"RW":"R", gfn,
		 *sptep, sptep);
	if (!was_rmapped && is_large_pte(*sptep))
		++vcpu->kvm->stat.lpages;

	if (is_shadow_present_pte(*sptep)) {
		if (!was_rmapped) {
			rmap_count = rmap_add(vcpu, sptep, gfn);
			if (rmap_count > RMAP_RECYCLE_THRESHOLD)
				rmap_recycle(vcpu, sptep, gfn);
		}
	}

	kvm_release_pfn_clean(pfn);
}
