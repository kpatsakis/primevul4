static bool __munlock_isolate_lru_page(struct page *page, bool getpage)
{
	if (PageLRU(page)) {
		struct lruvec *lruvec;

		lruvec = mem_cgroup_page_lruvec(page, page_zone(page));
		if (getpage)
			get_page(page);
		ClearPageLRU(page);
		del_page_from_lru_list(page, lruvec, page_lru(page));
		return true;
	}

	return false;
}
