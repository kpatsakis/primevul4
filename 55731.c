void putback_movable_pages(struct list_head *l)
{
	struct page *page;
	struct page *page2;

	list_for_each_entry_safe(page, page2, l, lru) {
		if (unlikely(PageHuge(page))) {
			putback_active_hugepage(page);
			continue;
		}
		list_del(&page->lru);
		/*
		 * We isolated non-lru movable page so here we can use
		 * __PageMovable because LRU page's mapping cannot have
		 * PAGE_MAPPING_MOVABLE.
		 */
		if (unlikely(__PageMovable(page))) {
			VM_BUG_ON_PAGE(!PageIsolated(page), page);
			lock_page(page);
			if (PageMovable(page))
				putback_movable_page(page);
			else
				__ClearPageIsolated(page);
			unlock_page(page);
			put_page(page);
		} else {
			dec_node_page_state(page, NR_ISOLATED_ANON +
					page_is_file_cache(page));
			putback_lru_page(page);
		}
	}
}
