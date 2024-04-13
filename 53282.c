static void smaps_pte_entry(pte_t *pte, unsigned long addr,
		struct mm_walk *walk)
{
	struct mem_size_stats *mss = walk->private;
	struct vm_area_struct *vma = walk->vma;
	struct page *page = NULL;

	if (pte_present(*pte)) {
		page = vm_normal_page(vma, addr, *pte);
	} else if (is_swap_pte(*pte)) {
		swp_entry_t swpent = pte_to_swp_entry(*pte);

		if (!non_swap_entry(swpent))
			mss->swap += PAGE_SIZE;
		else if (is_migration_entry(swpent))
			page = migration_entry_to_page(swpent);
	}

	if (!page)
		return;
	smaps_account(mss, page, PAGE_SIZE, pte_young(*pte), pte_dirty(*pte));
}
