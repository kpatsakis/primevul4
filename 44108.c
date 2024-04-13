void munlock_vma_pages_range(struct vm_area_struct *vma,
			     unsigned long start, unsigned long end)
{
	vma->vm_flags &= ~VM_LOCKED;

	while (start < end) {
		struct page *page = NULL;
		unsigned int page_mask;
		unsigned long page_increm;
		struct pagevec pvec;
		struct zone *zone;
		int zoneid;

		pagevec_init(&pvec, 0);
		/*
		 * Although FOLL_DUMP is intended for get_dump_page(),
		 * it just so happens that its special treatment of the
		 * ZERO_PAGE (returning an error instead of doing get_page)
		 * suits munlock very well (and if somehow an abnormal page
		 * has sneaked into the range, we won't oops here: great).
		 */
		page = follow_page_mask(vma, start, FOLL_GET | FOLL_DUMP,
				&page_mask);

		if (page && !IS_ERR(page)) {
			if (PageTransHuge(page)) {
				lock_page(page);
				/*
				 * Any THP page found by follow_page_mask() may
				 * have gotten split before reaching
				 * munlock_vma_page(), so we need to recompute
				 * the page_mask here.
				 */
				page_mask = munlock_vma_page(page);
				unlock_page(page);
				put_page(page); /* follow_page_mask() */
			} else {
				/*
				 * Non-huge pages are handled in batches via
				 * pagevec. The pin from follow_page_mask()
				 * prevents them from collapsing by THP.
				 */
				pagevec_add(&pvec, page);
				zone = page_zone(page);
				zoneid = page_zone_id(page);

				/*
				 * Try to fill the rest of pagevec using fast
				 * pte walk. This will also update start to
				 * the next page to process. Then munlock the
				 * pagevec.
				 */
				start = __munlock_pagevec_fill(&pvec, vma,
						zoneid, start, end);
				__munlock_pagevec(&pvec, zone);
				goto next;
			}
		}
		/* It's a bug to munlock in the middle of a THP page */
		VM_BUG_ON((start >> PAGE_SHIFT) & page_mask);
		page_increm = 1 + page_mask;
		start += page_increm * PAGE_SIZE;
next:
		cond_resched();
	}
}
