void page_remove_rmap(struct page *page)
{
	bool anon = PageAnon(page);
	bool locked;
	unsigned long flags;

	/*
	 * The anon case has no mem_cgroup page_stat to update; but may
	 * uncharge_page() below, where the lock ordering can deadlock if
	 * we hold the lock against page_stat move: so avoid it on anon.
	 */
	if (!anon)
		mem_cgroup_begin_update_page_stat(page, &locked, &flags);

	/* page still mapped by someone else? */
	if (!atomic_add_negative(-1, &page->_mapcount))
		goto out;

	/*
	 * Hugepages are not counted in NR_ANON_PAGES nor NR_FILE_MAPPED
	 * and not charged by memcg for now.
	 */
	if (unlikely(PageHuge(page)))
		goto out;
	if (anon) {
		mem_cgroup_uncharge_page(page);
		if (PageTransHuge(page))
			__dec_zone_page_state(page,
					      NR_ANON_TRANSPARENT_HUGEPAGES);
		__mod_zone_page_state(page_zone(page), NR_ANON_PAGES,
				-hpage_nr_pages(page));
	} else {
		__dec_zone_page_state(page, NR_FILE_MAPPED);
		mem_cgroup_dec_page_stat(page, MEM_CGROUP_STAT_FILE_MAPPED);
		mem_cgroup_end_update_page_stat(page, &locked, &flags);
	}
	if (unlikely(PageMlocked(page)))
		clear_page_mlock(page);
	/*
	 * It would be tidy to reset the PageAnon mapping here,
	 * but that might overwrite a racing page_add_anon_rmap
	 * which increments mapcount after us but sets mapping
	 * before us: so leave the reset to free_hot_cold_page,
	 * and remember that it's only reliable while mapped.
	 * Leaving it set also helps swapoff to reinstate ptes
	 * faster for those pages still in swapcache.
	 */
	return;
out:
	if (!anon)
		mem_cgroup_end_update_page_stat(page, &locked, &flags);
}
