static void var_mtrr_range(struct kvm_mtrr_range *range, u64 *start, u64 *end)
{
	u64 mask;

	*start = range->base & PAGE_MASK;

	mask = range->mask & PAGE_MASK;

	/* This cannot overflow because writing to the reserved bits of
	 * variable MTRRs causes a #GP.
	 */
	*end = (*start | ~mask) + 1;
}
