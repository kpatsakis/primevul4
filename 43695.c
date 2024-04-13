static inline bool is_last_gpte(struct kvm_mmu *mmu, unsigned level, unsigned gpte)
{
	unsigned index;

	index = level - 1;
	index |= (gpte & PT_PAGE_SIZE_MASK) >> (PT_PAGE_SIZE_SHIFT - 2);
	return mmu->last_pte_bitmap & (1 << index);
}
