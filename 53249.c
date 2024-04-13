static int gather_hugetlb_stats(pte_t *pte, unsigned long hmask,
		unsigned long addr, unsigned long end, struct mm_walk *walk)
{
	struct numa_maps *md;
	struct page *page;

	if (!pte_present(*pte))
		return 0;

	page = pte_page(*pte);
	if (!page)
		return 0;

	md = walk->private;
	gather_stats(page, md, pte_dirty(*pte), 1);
	return 0;
}
