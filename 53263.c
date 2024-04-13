static int pagemap_pte_range(pmd_t *pmd, unsigned long addr, unsigned long end,
			     struct mm_walk *walk)
{
	struct vm_area_struct *vma = walk->vma;
	struct pagemapread *pm = walk->private;
	spinlock_t *ptl;
	pte_t *pte, *orig_pte;
	int err = 0;

	if (pmd_trans_huge_lock(pmd, vma, &ptl) == 1) {
		int pmd_flags2;

		if ((vma->vm_flags & VM_SOFTDIRTY) || pmd_soft_dirty(*pmd))
			pmd_flags2 = __PM_SOFT_DIRTY;
		else
			pmd_flags2 = 0;

		for (; addr != end; addr += PAGE_SIZE) {
			unsigned long offset;
			pagemap_entry_t pme;

			offset = (addr & ~PAGEMAP_WALK_MASK) >>
					PAGE_SHIFT;
			thp_pmd_to_pagemap_entry(&pme, pm, *pmd, offset, pmd_flags2);
			err = add_to_pagemap(addr, &pme, pm);
			if (err)
				break;
		}
		spin_unlock(ptl);
		return err;
	}

	if (pmd_trans_unstable(pmd))
		return 0;

	/*
	 * We can assume that @vma always points to a valid one and @end never
	 * goes beyond vma->vm_end.
	 */
	orig_pte = pte = pte_offset_map_lock(walk->mm, pmd, addr, &ptl);
	for (; addr < end; pte++, addr += PAGE_SIZE) {
		pagemap_entry_t pme;

		pte_to_pagemap_entry(&pme, pm, vma, addr, *pte);
		err = add_to_pagemap(addr, &pme, pm);
		if (err)
			break;
	}
	pte_unmap_unlock(orig_pte, ptl);

	cond_resched();

	return err;
}
