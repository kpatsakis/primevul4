static u64 generation_mmio_spte_mask(unsigned int gen)
{
	u64 mask;

	WARN_ON(gen > MMIO_MAX_GEN);

	mask = (gen & MMIO_GEN_LOW_MASK) << MMIO_SPTE_GEN_LOW_SHIFT;
	mask |= ((u64)gen >> MMIO_GEN_LOW_SHIFT) << MMIO_SPTE_GEN_HIGH_SHIFT;
	return mask;
}
