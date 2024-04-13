static int is_shadow_present_pte(u64 pte)
{
	return pte & PT_PRESENT_MASK && !is_mmio_spte(pte);
}
