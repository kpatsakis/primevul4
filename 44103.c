static bool __putback_lru_fast_prepare(struct page *page, struct pagevec *pvec,
		int *pgrescued)
{
	VM_BUG_ON_PAGE(PageLRU(page), page);
	VM_BUG_ON_PAGE(!PageLocked(page), page);

	if (page_mapcount(page) <= 1 && page_evictable(page)) {
		pagevec_add(pvec, page);
		if (TestClearPageUnevictable(page))
			(*pgrescued)++;
		unlock_page(page);
		return true;
	}

	return false;
}
