void page_add_new_anon_rmap(struct page *page,
	struct vm_area_struct *vma, unsigned long address)
{
	VM_BUG_ON(address < vma->vm_start || address >= vma->vm_end);
	SetPageSwapBacked(page);
	atomic_set(&page->_mapcount, 0); /* increment count (starts at -1) */
	if (PageTransHuge(page))
		__inc_zone_page_state(page, NR_ANON_TRANSPARENT_HUGEPAGES);
	__mod_zone_page_state(page_zone(page), NR_ANON_PAGES,
			hpage_nr_pages(page));
	__page_set_anon_rmap(page, vma, address, 1);
	if (!mlocked_vma_newpage(vma, page)) {
		SetPageActive(page);
		lru_cache_add(page);
	} else
		add_page_to_unevictable_list(page);
}
