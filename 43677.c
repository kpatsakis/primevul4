static unsigned int get_mmio_spte_generation(u64 spte)
{
	unsigned int gen;

	spte &= ~shadow_mmio_mask;

	gen = (spte >> MMIO_SPTE_GEN_LOW_SHIFT) & MMIO_GEN_LOW_MASK;
	gen |= (spte >> MMIO_SPTE_GEN_HIGH_SHIFT) << MMIO_GEN_LOW_SHIFT;
	return gen;
}
