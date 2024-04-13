int try_to_unmap(struct page *page, enum ttu_flags flags)
{
	int ret;
	struct rmap_walk_control rwc = {
		.rmap_one = try_to_unmap_one,
		.arg = (void *)flags,
		.done = page_not_mapped,
		.file_nonlinear = try_to_unmap_nonlinear,
		.anon_lock = page_lock_anon_vma_read,
	};

	VM_BUG_ON_PAGE(!PageHuge(page) && PageTransHuge(page), page);

	/*
	 * During exec, a temporary VMA is setup and later moved.
	 * The VMA is moved under the anon_vma lock but not the
	 * page tables leading to a race where migration cannot
	 * find the migration ptes. Rather than increasing the
	 * locking requirements of exec(), migration skips
	 * temporary VMAs until after exec() completes.
	 */
	if (flags & TTU_MIGRATION && !PageKsm(page) && PageAnon(page))
		rwc.invalid_vma = invalid_migration_vma;

	ret = rmap_walk(page, &rwc);

	if (ret != SWAP_MLOCK && !page_mapped(page))
		ret = SWAP_SUCCESS;
	return ret;
}
