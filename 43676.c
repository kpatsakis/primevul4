static unsigned get_mmio_spte_access(u64 spte)
{
	u64 mask = generation_mmio_spte_mask(MMIO_MAX_GEN) | shadow_mmio_mask;
	return (spte & ~mask) & ~PAGE_MASK;
}
