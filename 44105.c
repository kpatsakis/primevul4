void clear_page_mlock(struct page *page)
{
	if (!TestClearPageMlocked(page))
		return;

	mod_zone_page_state(page_zone(page), NR_MLOCK,
			    -hpage_nr_pages(page));
	count_vm_event(UNEVICTABLE_PGCLEARED);
	if (!isolate_lru_page(page)) {
		putback_lru_page(page);
	} else {
		/*
		 * We lost the race. the page already moved to evictable list.
		 */
		if (PageUnevictable(page))
			count_vm_event(UNEVICTABLE_PGSTRANDED);
	}
}
