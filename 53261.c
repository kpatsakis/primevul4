static int pagemap_hugetlb_range(pte_t *pte, unsigned long hmask,
				 unsigned long addr, unsigned long end,
				 struct mm_walk *walk)
{
	struct pagemapread *pm = walk->private;
	struct vm_area_struct *vma = walk->vma;
	int err = 0;
	int flags2;
	pagemap_entry_t pme;

	if (vma->vm_flags & VM_SOFTDIRTY)
		flags2 = __PM_SOFT_DIRTY;
	else
		flags2 = 0;

	for (; addr != end; addr += PAGE_SIZE) {
		int offset = (addr & ~hmask) >> PAGE_SHIFT;
		huge_pte_to_pagemap_entry(&pme, pm, *pte, offset, flags2);
		err = add_to_pagemap(addr, &pme, pm);
		if (err)
			return err;
	}

	cond_resched();

	return err;
}
