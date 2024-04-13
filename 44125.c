void do_page_add_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address, int exclusive)
{
	int first = atomic_inc_and_test(&page->_mapcount);
	if (first) {
		if (PageTransHuge(page))
			__inc_zone_page_state(page,
					      NR_ANON_TRANSPARENT_HUGEPAGES);
		__mod_zone_page_state(page_zone(page), NR_ANON_PAGES,
				hpage_nr_pages(page));
	}
	if (unlikely(PageKsm(page)))
		return;

	VM_BUG_ON_PAGE(!PageLocked(page), page);
	/* address might be in next vma when migration races vma_adjust */
	if (first)
		__page_set_anon_rmap(page, vma, address, exclusive);
	else
		__page_check_anon_rmap(page, vma, address);
}
