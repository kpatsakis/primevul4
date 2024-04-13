int shmem_unuse(swp_entry_t swap, struct page *page)
{
	struct list_head *this, *next;
	struct shmem_inode_info *info;
	int found = 0;
	int error = 0;

	/*
	 * There's a faint possibility that swap page was replaced before
	 * caller locked it: caller will come back later with the right page.
	 */
	if (unlikely(!PageSwapCache(page) || page_private(page) != swap.val))
		goto out;

	/*
	 * Charge page using GFP_KERNEL while we can wait, before taking
	 * the shmem_swaplist_mutex which might hold up shmem_writepage().
	 * Charged back to the user (not to caller) when swap account is used.
	 */
	error = mem_cgroup_cache_charge(page, current->mm, GFP_KERNEL);
	if (error)
		goto out;
	/* No radix_tree_preload: swap entry keeps a place for page in tree */

	mutex_lock(&shmem_swaplist_mutex);
	list_for_each_safe(this, next, &shmem_swaplist) {
		info = list_entry(this, struct shmem_inode_info, swaplist);
		if (info->swapped)
			found = shmem_unuse_inode(info, swap, &page);
		else
			list_del_init(&info->swaplist);
		cond_resched();
		if (found)
			break;
	}
	mutex_unlock(&shmem_swaplist_mutex);

	if (found < 0)
		error = found;
out:
	unlock_page(page);
	page_cache_release(page);
	return error;
}
