static int mmu_spte_clear_track_bits(u64 *sptep)
{
	pfn_t pfn;
	u64 old_spte = *sptep;

	if (!spte_has_volatile_bits(old_spte))
		__update_clear_spte_fast(sptep, 0ull);
	else
		old_spte = __update_clear_spte_slow(sptep, 0ull);

	if (!is_rmap_spte(old_spte))
		return 0;

	pfn = spte_to_pfn(old_spte);

	/*
	 * KVM does not hold the refcount of the page used by
	 * kvm mmu, before reclaiming the page, we should
	 * unmap it from mmu first.
	 */
	WARN_ON(!kvm_is_mmio_pfn(pfn) && !page_count(pfn_to_page(pfn)));

	if (!shadow_accessed_mask || old_spte & shadow_accessed_mask)
		kvm_set_pfn_accessed(pfn);
	if (!shadow_dirty_mask || (old_spte & shadow_dirty_mask))
		kvm_set_pfn_dirty(pfn);
	return 1;
}
