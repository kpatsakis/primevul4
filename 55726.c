int migrate_page_move_mapping(struct address_space *mapping,
		struct page *newpage, struct page *page,
		struct buffer_head *head, enum migrate_mode mode,
		int extra_count)
{
	struct zone *oldzone, *newzone;
	int dirty;
	int expected_count = 1 + extra_count;
	void **pslot;

	if (!mapping) {
		/* Anonymous page without mapping */
		if (page_count(page) != expected_count)
			return -EAGAIN;

		/* No turning back from here */
		newpage->index = page->index;
		newpage->mapping = page->mapping;
		if (PageSwapBacked(page))
			__SetPageSwapBacked(newpage);

		return MIGRATEPAGE_SUCCESS;
	}

	oldzone = page_zone(page);
	newzone = page_zone(newpage);

	spin_lock_irq(&mapping->tree_lock);

	pslot = radix_tree_lookup_slot(&mapping->page_tree,
 					page_index(page));

	expected_count += 1 + page_has_private(page);
	if (page_count(page) != expected_count ||
		radix_tree_deref_slot_protected(pslot, &mapping->tree_lock) != page) {
		spin_unlock_irq(&mapping->tree_lock);
		return -EAGAIN;
	}

	if (!page_ref_freeze(page, expected_count)) {
		spin_unlock_irq(&mapping->tree_lock);
		return -EAGAIN;
	}

	/*
	 * In the async migration case of moving a page with buffers, lock the
	 * buffers using trylock before the mapping is moved. If the mapping
	 * was moved, we later failed to lock the buffers and could not move
	 * the mapping back due to an elevated page count, we would have to
	 * block waiting on other references to be dropped.
	 */
	if (mode == MIGRATE_ASYNC && head &&
			!buffer_migrate_lock_buffers(head, mode)) {
		page_ref_unfreeze(page, expected_count);
		spin_unlock_irq(&mapping->tree_lock);
		return -EAGAIN;
	}

	/*
	 * Now we know that no one else is looking at the page:
	 * no turning back from here.
	 */
	newpage->index = page->index;
	newpage->mapping = page->mapping;
	get_page(newpage);	/* add cache reference */
	if (PageSwapBacked(page)) {
		__SetPageSwapBacked(newpage);
		if (PageSwapCache(page)) {
			SetPageSwapCache(newpage);
			set_page_private(newpage, page_private(page));
		}
	} else {
		VM_BUG_ON_PAGE(PageSwapCache(page), page);
	}

	/* Move dirty while page refs frozen and newpage not yet exposed */
	dirty = PageDirty(page);
	if (dirty) {
		ClearPageDirty(page);
		SetPageDirty(newpage);
	}

	radix_tree_replace_slot(&mapping->page_tree, pslot, newpage);

	/*
	 * Drop cache reference from old page by unfreezing
	 * to one less reference.
	 * We know this isn't the last reference.
	 */
	page_ref_unfreeze(page, expected_count - 1);

	spin_unlock(&mapping->tree_lock);
	/* Leave irq disabled to prevent preemption while updating stats */

	/*
	 * If moved to a different zone then also account
	 * the page for that zone. Other VM counters will be
	 * taken care of when we establish references to the
	 * new page and drop references to the old page.
	 *
	 * Note that anonymous pages are accounted for
	 * via NR_FILE_PAGES and NR_ANON_MAPPED if they
	 * are mapped to swap space.
	 */
	if (newzone != oldzone) {
		__dec_node_state(oldzone->zone_pgdat, NR_FILE_PAGES);
		__inc_node_state(newzone->zone_pgdat, NR_FILE_PAGES);
		if (PageSwapBacked(page) && !PageSwapCache(page)) {
			__dec_node_state(oldzone->zone_pgdat, NR_SHMEM);
			__inc_node_state(newzone->zone_pgdat, NR_SHMEM);
		}
		if (dirty && mapping_cap_account_dirty(mapping)) {
			__dec_node_state(oldzone->zone_pgdat, NR_FILE_DIRTY);
			__dec_zone_state(oldzone, NR_ZONE_WRITE_PENDING);
			__inc_node_state(newzone->zone_pgdat, NR_FILE_DIRTY);
			__inc_zone_state(newzone, NR_ZONE_WRITE_PENDING);
		}
	}
	local_irq_enable();

	return MIGRATEPAGE_SUCCESS;
}
