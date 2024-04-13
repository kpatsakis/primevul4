static void smaps_pmd_entry(pmd_t *pmd, unsigned long addr,
		struct mm_walk *walk)
{
	struct mem_size_stats *mss = walk->private;
	struct vm_area_struct *vma = walk->vma;
	struct page *page;

	/* FOLL_DUMP will return -EFAULT on huge zero page */
	page = follow_trans_huge_pmd(vma, addr, pmd, FOLL_DUMP);
	if (IS_ERR_OR_NULL(page))
		return;
	mss->anonymous_thp += HPAGE_PMD_SIZE;
	smaps_account(mss, page, HPAGE_PMD_SIZE,
			pmd_young(*pmd), pmd_dirty(*pmd));
}
