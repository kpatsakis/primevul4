static int gather_pte_stats(pmd_t *pmd, unsigned long addr,
		unsigned long end, struct mm_walk *walk)
{
	struct numa_maps *md = walk->private;
	struct vm_area_struct *vma = walk->vma;
	spinlock_t *ptl;
	pte_t *orig_pte;
	pte_t *pte;

	if (pmd_trans_huge_lock(pmd, vma, &ptl) == 1) {
		pte_t huge_pte = *(pte_t *)pmd;
		struct page *page;

		page = can_gather_numa_stats(huge_pte, vma, addr);
		if (page)
			gather_stats(page, md, pte_dirty(huge_pte),
				     HPAGE_PMD_SIZE/PAGE_SIZE);
		spin_unlock(ptl);
		return 0;
	}

	if (pmd_trans_unstable(pmd))
		return 0;
	orig_pte = pte = pte_offset_map_lock(walk->mm, pmd, addr, &ptl);
	do {
		struct page *page = can_gather_numa_stats(*pte, vma, addr);
		if (!page)
			continue;
		gather_stats(page, md, pte_dirty(*pte), 1);

	} while (pte++, addr += PAGE_SIZE, addr != end);
	pte_unmap_unlock(orig_pte, ptl);
	return 0;
}
