static int shmem_replace_page(struct page **pagep, gfp_t gfp,
				struct shmem_inode_info *info, pgoff_t index)
{
	struct page *oldpage, *newpage;
	struct address_space *swap_mapping;
	pgoff_t swap_index;
	int error;

	oldpage = *pagep;
	swap_index = page_private(oldpage);
	swap_mapping = page_mapping(oldpage);

	/*
	 * We have arrived here because our zones are constrained, so don't
	 * limit chance of success by further cpuset and node constraints.
	 */
	gfp &= ~GFP_CONSTRAINT_MASK;
	newpage = shmem_alloc_page(gfp, info, index);
	if (!newpage)
		return -ENOMEM;

	page_cache_get(newpage);
	copy_highpage(newpage, oldpage);
	flush_dcache_page(newpage);

	__set_page_locked(newpage);
	SetPageUptodate(newpage);
	SetPageSwapBacked(newpage);
	set_page_private(newpage, swap_index);
	SetPageSwapCache(newpage);

	/*
	 * Our caller will very soon move newpage out of swapcache, but it's
	 * a nice clean interface for us to replace oldpage by newpage there.
	 */
	spin_lock_irq(&swap_mapping->tree_lock);
	error = shmem_radix_tree_replace(swap_mapping, swap_index, oldpage,
								   newpage);
	if (!error) {
		__inc_zone_page_state(newpage, NR_FILE_PAGES);
		__dec_zone_page_state(oldpage, NR_FILE_PAGES);
	}
	spin_unlock_irq(&swap_mapping->tree_lock);

	if (unlikely(error)) {
		/*
		 * Is this possible?  I think not, now that our callers check
		 * both PageSwapCache and page_private after getting page lock;
		 * but be defensive.  Reverse old to newpage for clear and free.
		 */
		oldpage = newpage;
	} else {
		mem_cgroup_replace_page_cache(oldpage, newpage);
		lru_cache_add_anon(newpage);
		*pagep = newpage;
	}

	ClearPageSwapCache(oldpage);
	set_page_private(oldpage, 0);

	unlock_page(oldpage);
	page_cache_release(oldpage);
	page_cache_release(oldpage);
	return error;
}
