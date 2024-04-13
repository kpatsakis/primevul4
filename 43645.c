static bool __check_direct_spte_mmio_pf(u64 spte)
{
	/* It is valid if the spte is zapped. */
	return spte == 0ull;
}
