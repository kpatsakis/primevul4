bool btrfs_page_exists_in_range(struct inode *inode, loff_t start, loff_t end)
{
	struct radix_tree_root *root = &inode->i_mapping->page_tree;
	int found = false;
	void **pagep = NULL;
	struct page *page = NULL;
	int start_idx;
	int end_idx;

	start_idx = start >> PAGE_CACHE_SHIFT;

	/*
	 * end is the last byte in the last page.  end == start is legal
	 */
	end_idx = end >> PAGE_CACHE_SHIFT;

	rcu_read_lock();

	/* Most of the code in this while loop is lifted from
	 * find_get_page.  It's been modified to begin searching from a
	 * page and return just the first page found in that range.  If the
	 * found idx is less than or equal to the end idx then we know that
	 * a page exists.  If no pages are found or if those pages are
	 * outside of the range then we're fine (yay!) */
	while (page == NULL &&
	       radix_tree_gang_lookup_slot(root, &pagep, NULL, start_idx, 1)) {
		page = radix_tree_deref_slot(pagep);
		if (unlikely(!page))
			break;

		if (radix_tree_exception(page)) {
			if (radix_tree_deref_retry(page)) {
				page = NULL;
				continue;
			}
			/*
			 * Otherwise, shmem/tmpfs must be storing a swap entry
			 * here as an exceptional entry: so return it without
			 * attempting to raise page count.
			 */
			page = NULL;
			break; /* TODO: Is this relevant for this use case? */
		}

		if (!page_cache_get_speculative(page)) {
			page = NULL;
			continue;
		}

		/*
		 * Has the page moved?
		 * This is part of the lockless pagecache protocol. See
		 * include/linux/pagemap.h for details.
		 */
		if (unlikely(page != *pagep)) {
			page_cache_release(page);
			page = NULL;
		}
	}

	if (page) {
		if (page->index <= end_idx)
			found = true;
		page_cache_release(page);
	}

	rcu_read_unlock();
	return found;
}
