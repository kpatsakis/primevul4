int try_to_munlock(struct page *page)
{
	int ret;
	struct rmap_walk_control rwc = {
		.rmap_one = try_to_unmap_one,
		.arg = (void *)TTU_MUNLOCK,
		.done = page_not_mapped,
		/*
		 * We don't bother to try to find the munlocked page in
		 * nonlinears. It's costly. Instead, later, page reclaim logic
		 * may call try_to_unmap() and recover PG_mlocked lazily.
		 */
		.file_nonlinear = NULL,
		.anon_lock = page_lock_anon_vma_read,

	};

	VM_BUG_ON_PAGE(!PageLocked(page) || PageLRU(page), page);

	ret = rmap_walk(page, &rwc);
	return ret;
}
