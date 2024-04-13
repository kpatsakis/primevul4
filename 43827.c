static bool spte_has_volatile_bits(u64 spte)
{
	/*
	 * Always atomicly update spte if it can be updated
	 * out of mmu-lock, it can ensure dirty bit is not lost,
	 * also, it can help us to get a stable is_writable_pte()
	 * to ensure tlb flush is not missed.
	 */
	if (spte_is_locklessly_modifiable(spte))
		return true;

	if (!shadow_accessed_mask)
		return false;

	if (!is_shadow_present_pte(spte))
		return false;

	if ((spte & shadow_accessed_mask) &&
	      (!is_writable_pte(spte) || (spte & shadow_dirty_mask)))
		return false;

	return true;
}
