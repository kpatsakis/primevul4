static unsigned shmem_find_get_pages_and_swap(struct address_space *mapping,
					pgoff_t start, unsigned int nr_pages,
					struct page **pages, pgoff_t *indices)
{
	void **slot;
	unsigned int ret = 0;
	struct radix_tree_iter iter;

	if (!nr_pages)
		return 0;

	rcu_read_lock();
restart:
	radix_tree_for_each_slot(slot, &mapping->page_tree, &iter, start) {
		struct page *page;
repeat:
		page = radix_tree_deref_slot(slot);
		if (unlikely(!page))
			continue;
		if (radix_tree_exception(page)) {
			if (radix_tree_deref_retry(page))
				goto restart;
			/*
			 * Otherwise, we must be storing a swap entry
			 * here as an exceptional entry: so return it
			 * without attempting to raise page count.
			 */
			goto export;
		}
		if (!page_cache_get_speculative(page))
			goto repeat;

		/* Has the page moved? */
		if (unlikely(page != *slot)) {
			page_cache_release(page);
			goto repeat;
		}
export:
		indices[ret] = iter.index;
		pages[ret] = page;
		if (++ret == nr_pages)
			break;
	}
	rcu_read_unlock();
	return ret;
}
