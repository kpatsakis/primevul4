static int is_large_pte(u64 pte)
{
	return pte & PT_PAGE_SIZE_MASK;
}
