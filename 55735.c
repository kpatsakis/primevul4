static int unmap_and_move_huge_page(new_page_t get_new_page,
				free_page_t put_new_page, unsigned long private,
				struct page *hpage, int force,
				enum migrate_mode mode, int reason)
{
	int rc = -EAGAIN;
	int *result = NULL;
	int page_was_mapped = 0;
	struct page *new_hpage;
	struct anon_vma *anon_vma = NULL;

	/*
	 * Movability of hugepages depends on architectures and hugepage size.
	 * This check is necessary because some callers of hugepage migration
	 * like soft offline and memory hotremove don't walk through page
	 * tables or check whether the hugepage is pmd-based or not before
	 * kicking migration.
	 */
	if (!hugepage_migration_supported(page_hstate(hpage))) {
		putback_active_hugepage(hpage);
		return -ENOSYS;
	}

	new_hpage = get_new_page(hpage, private, &result);
	if (!new_hpage)
		return -ENOMEM;

	if (!trylock_page(hpage)) {
		if (!force || mode != MIGRATE_SYNC)
			goto out;
		lock_page(hpage);
	}

	if (PageAnon(hpage))
		anon_vma = page_get_anon_vma(hpage);

	if (unlikely(!trylock_page(new_hpage)))
		goto put_anon;

	if (page_mapped(hpage)) {
		try_to_unmap(hpage,
			TTU_MIGRATION|TTU_IGNORE_MLOCK|TTU_IGNORE_ACCESS);
		page_was_mapped = 1;
	}

	if (!page_mapped(hpage))
		rc = move_to_new_page(new_hpage, hpage, mode);

	if (page_was_mapped)
		remove_migration_ptes(hpage,
			rc == MIGRATEPAGE_SUCCESS ? new_hpage : hpage, false);

	unlock_page(new_hpage);

put_anon:
	if (anon_vma)
		put_anon_vma(anon_vma);

	if (rc == MIGRATEPAGE_SUCCESS) {
		hugetlb_cgroup_migrate(hpage, new_hpage);
		put_new_page = NULL;
		set_page_owner_migrate_reason(new_hpage, reason);
	}

	unlock_page(hpage);
out:
	if (rc != -EAGAIN)
		putback_active_hugepage(hpage);
	if (reason == MR_MEMORY_FAILURE && !test_set_page_hwpoison(hpage))
		num_poisoned_pages_inc();

	/*
	 * If migration was not successful and there's a freeing callback, use
	 * it.  Otherwise, put_page() will drop the reference grabbed during
	 * isolation.
	 */
	if (put_new_page)
		put_new_page(new_hpage, private);
	else
		putback_active_hugepage(new_hpage);

	if (result) {
		if (rc)
			*result = rc;
		else
			*result = page_to_nid(new_hpage);
	}
	return rc;
}
