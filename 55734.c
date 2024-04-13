static ICE_noinline int unmap_and_move(new_page_t get_new_page,
				   free_page_t put_new_page,
				   unsigned long private, struct page *page,
				   int force, enum migrate_mode mode,
				   enum migrate_reason reason)
{
	int rc = MIGRATEPAGE_SUCCESS;
	int *result = NULL;
	struct page *newpage;

	newpage = get_new_page(page, private, &result);
	if (!newpage)
		return -ENOMEM;

	if (page_count(page) == 1) {
		/* page was freed from under us. So we are done. */
		ClearPageActive(page);
		ClearPageUnevictable(page);
		if (unlikely(__PageMovable(page))) {
			lock_page(page);
			if (!PageMovable(page))
				__ClearPageIsolated(page);
			unlock_page(page);
		}
		if (put_new_page)
			put_new_page(newpage, private);
		else
			put_page(newpage);
		goto out;
	}

	if (unlikely(PageTransHuge(page))) {
		lock_page(page);
		rc = split_huge_page(page);
		unlock_page(page);
		if (rc)
			goto out;
	}

	rc = __unmap_and_move(page, newpage, force, mode);
	if (rc == MIGRATEPAGE_SUCCESS)
		set_page_owner_migrate_reason(newpage, reason);

out:
	if (rc != -EAGAIN) {
		/*
		 * A page that has been migrated has all references
		 * removed and will be freed. A page that has not been
		 * migrated will have kepts its references and be
		 * restored.
		 */
		list_del(&page->lru);

		/*
		 * Compaction can migrate also non-LRU pages which are
		 * not accounted to NR_ISOLATED_*. They can be recognized
		 * as __PageMovable
		 */
		if (likely(!__PageMovable(page)))
			dec_node_page_state(page, NR_ISOLATED_ANON +
					page_is_file_cache(page));
	}

	/*
	 * If migration is successful, releases reference grabbed during
	 * isolation. Otherwise, restore the page to right list unless
	 * we want to retry.
	 */
	if (rc == MIGRATEPAGE_SUCCESS) {
		put_page(page);
		if (reason == MR_MEMORY_FAILURE) {
			/*
			 * Set PG_HWPoison on just freed page
			 * intentionally. Although it's rather weird,
			 * it's how HWPoison flag works at the moment.
			 */
			if (!test_set_page_hwpoison(page))
				num_poisoned_pages_inc();
		}
	} else {
		if (rc != -EAGAIN) {
			if (likely(!__PageMovable(page))) {
				putback_lru_page(page);
				goto put_new;
			}

			lock_page(page);
			if (PageMovable(page))
				putback_movable_page(page);
			else
				__ClearPageIsolated(page);
			unlock_page(page);
			put_page(page);
		}
put_new:
		if (put_new_page)
			put_new_page(newpage, private);
		else
			put_page(newpage);
	}

	if (result) {
		if (rc)
			*result = rc;
		else
			*result = page_to_nid(newpage);
	}
	return rc;
}
