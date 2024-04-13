static unsigned long __munlock_pagevec_fill(struct pagevec *pvec,
		struct vm_area_struct *vma, int zoneid,	unsigned long start,
		unsigned long end)
{
	pte_t *pte;
	spinlock_t *ptl;

	/*
	 * Initialize pte walk starting at the already pinned page where we
	 * are sure that there is a pte, as it was pinned under the same
	 * mmap_sem write op.
	 */
	pte = get_locked_pte(vma->vm_mm, start,	&ptl);
	/* Make sure we do not cross the page table boundary */
	end = pgd_addr_end(start, end);
	end = pud_addr_end(start, end);
	end = pmd_addr_end(start, end);

	/* The page next to the pinned page is the first we will try to get */
	start += PAGE_SIZE;
	while (start < end) {
		struct page *page = NULL;
		pte++;
		if (pte_present(*pte))
			page = vm_normal_page(vma, start, *pte);
		/*
		 * Break if page could not be obtained or the page's node+zone does not
		 * match
		 */
		if (!page || page_zone_id(page) != zoneid)
			break;

		get_page(page);
		/*
		 * Increase the address that will be returned *before* the
		 * eventual break due to pvec becoming full by adding the page
		 */
		start += PAGE_SIZE;
		if (pagevec_add(pvec, page) == 0)
			break;
	}
	pte_unmap_unlock(pte, ptl);
	return start;
}
