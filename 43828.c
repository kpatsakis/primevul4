static bool spte_is_bit_cleared(u64 old_spte, u64 new_spte, u64 bit_mask)
{
	return (old_spte & bit_mask) && !(new_spte & bit_mask);
}
