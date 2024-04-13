static int kvm_set_pte_rmapp(struct kvm *kvm, unsigned long *rmapp,
			     struct kvm_memory_slot *slot, unsigned long data)
{
	u64 *sptep;
	struct rmap_iterator iter;
	int need_flush = 0;
	u64 new_spte;
	pte_t *ptep = (pte_t *)data;
	pfn_t new_pfn;

	WARN_ON(pte_huge(*ptep));
	new_pfn = pte_pfn(*ptep);

	for (sptep = rmap_get_first(*rmapp, &iter); sptep;) {
		BUG_ON(!is_shadow_present_pte(*sptep));
		rmap_printk("kvm_set_pte_rmapp: spte %p %llx\n", sptep, *sptep);

		need_flush = 1;

		if (pte_write(*ptep)) {
			drop_spte(kvm, sptep);
			sptep = rmap_get_first(*rmapp, &iter);
		} else {
			new_spte = *sptep & ~PT64_BASE_ADDR_MASK;
			new_spte |= (u64)new_pfn << PAGE_SHIFT;

			new_spte &= ~PT_WRITABLE_MASK;
			new_spte &= ~SPTE_HOST_WRITEABLE;
			new_spte &= ~shadow_accessed_mask;

			mmu_spte_clear_track_bits(sptep);
			mmu_spte_set(sptep, new_spte);
			sptep = rmap_get_next(&iter);
		}
	}

	if (need_flush)
		kvm_flush_remote_tlbs(kvm);

	return 0;
}
