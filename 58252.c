static void add_pfn_range_mapped(unsigned long start_pfn, unsigned long end_pfn)
{
	nr_pfn_mapped = add_range_with_merge(pfn_mapped, E820_X_MAX,
					     nr_pfn_mapped, start_pfn, end_pfn);
	nr_pfn_mapped = clean_sort_range(pfn_mapped, E820_X_MAX);

	max_pfn_mapped = max(max_pfn_mapped, end_pfn);

	if (start_pfn < (1UL<<(32-PAGE_SHIFT)))
		max_low_pfn_mapped = max(max_low_pfn_mapped,
					 min(end_pfn, 1UL<<(32-PAGE_SHIFT)));
}
