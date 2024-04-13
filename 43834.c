static void transparent_hugepage_adjust(struct kvm_vcpu *vcpu,
					gfn_t *gfnp, pfn_t *pfnp, int *levelp)
{
	pfn_t pfn = *pfnp;
	gfn_t gfn = *gfnp;
	int level = *levelp;

	/*
	 * Check if it's a transparent hugepage. If this would be an
	 * hugetlbfs page, level wouldn't be set to
	 * PT_PAGE_TABLE_LEVEL and there would be no adjustment done
	 * here.
	 */
	if (!is_error_noslot_pfn(pfn) && !kvm_is_mmio_pfn(pfn) &&
	    level == PT_PAGE_TABLE_LEVEL &&
	    PageTransCompound(pfn_to_page(pfn)) &&
	    !has_wrprotected_page(vcpu->kvm, gfn, PT_DIRECTORY_LEVEL)) {
		unsigned long mask;
		/*
		 * mmu_notifier_retry was successful and we hold the
		 * mmu_lock here, so the pmd can't become splitting
		 * from under us, and in turn
		 * __split_huge_page_refcount() can't run from under
		 * us and we can safely transfer the refcount from
		 * PG_tail to PG_head as we switch the pfn to tail to
		 * head.
		 */
		*levelp = level = PT_DIRECTORY_LEVEL;
		mask = KVM_PAGES_PER_HPAGE(level) - 1;
		VM_BUG_ON((gfn & mask) != (pfn & mask));
		if (pfn & mask) {
			gfn &= ~mask;
			*gfnp = gfn;
			kvm_release_pfn_clean(pfn);
			pfn &= ~mask;
			kvm_get_pfn(pfn);
			*pfnp = pfn;
		}
	}
}
