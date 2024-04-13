static bool mmu_spte_update(u64 *sptep, u64 new_spte)
{
	u64 old_spte = *sptep;
	bool ret = false;

	WARN_ON(!is_rmap_spte(new_spte));

	if (!is_shadow_present_pte(old_spte)) {
		mmu_spte_set(sptep, new_spte);
		return ret;
	}

	if (!spte_has_volatile_bits(old_spte))
		__update_clear_spte_fast(sptep, new_spte);
	else
		old_spte = __update_clear_spte_slow(sptep, new_spte);

	/*
	 * For the spte updated out of mmu-lock is safe, since
	 * we always atomicly update it, see the comments in
	 * spte_has_volatile_bits().
	 */
	if (is_writable_pte(old_spte) && !is_writable_pte(new_spte))
		ret = true;

	if (!shadow_accessed_mask)
		return ret;

	if (spte_is_bit_cleared(old_spte, new_spte, shadow_accessed_mask))
		kvm_set_pfn_accessed(spte_to_pfn(old_spte));
	if (spte_is_bit_cleared(old_spte, new_spte, shadow_dirty_mask))
		kvm_set_pfn_dirty(spte_to_pfn(old_spte));

	return ret;
}
