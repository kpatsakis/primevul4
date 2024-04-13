static int __unmap_and_move(struct page *page, struct page *newpage,
				int force, enum migrate_mode mode)
{
	int rc = -EAGAIN;
	int page_was_mapped = 0;
	struct anon_vma *anon_vma = NULL;
	bool is_lru = !__PageMovable(page);

	if (!trylock_page(page)) {
		if (!force || mode == MIGRATE_ASYNC)
			goto out;

		/*
		 * It's not safe for direct compaction to call lock_page.
		 * For example, during page readahead pages are added locked
		 * to the LRU. Later, when the IO completes the pages are
		 * marked uptodate and unlocked. However, the queueing
		 * could be merging multiple pages for one bio (e.g.
		 * mpage_readpages). If an allocation happens for the
		 * second or third page, the process can end up locking
		 * the same page twice and deadlocking. Rather than
		 * trying to be clever about what pages can be locked,
		 * avoid the use of lock_page for direct compaction
		 * altogether.
		 */
		if (current->flags & PF_MEMALLOC)
			goto out;

		lock_page(page);
	}

	if (PageWriteback(page)) {
		/*
		 * Only in the case of a full synchronous migration is it
		 * necessary to wait for PageWriteback. In the async case,
		 * the retry loop is too short and in the sync-light case,
		 * the overhead of stalling is too much
		 */
		if (mode != MIGRATE_SYNC) {
			rc = -EBUSY;
			goto out_unlock;
		}
		if (!force)
			goto out_unlock;
		wait_on_page_writeback(page);
	}

	/*
	 * By try_to_unmap(), page->mapcount goes down to 0 here. In this case,
	 * we cannot notice that anon_vma is freed while we migrates a page.
	 * This get_anon_vma() delays freeing anon_vma pointer until the end
	 * of migration. File cache pages are no problem because of page_lock()
	 * File Caches may use write_page() or lock_page() in migration, then,
	 * just care Anon page here.
	 *
	 * Only page_get_anon_vma() understands the subtleties of
	 * getting a hold on an anon_vma from outside one of its mms.
	 * But if we cannot get anon_vma, then we won't need it anyway,
	 * because that implies that the anon page is no longer mapped
	 * (and cannot be remapped so long as we hold the page lock).
	 */
	if (PageAnon(page) && !PageKsm(page))
		anon_vma = page_get_anon_vma(page);

	/*
	 * Block others from accessing the new page when we get around to
	 * establishing additional references. We are usually the only one
	 * holding a reference to newpage at this point. We used to have a BUG
	 * here if trylock_page(newpage) fails, but would like to allow for
	 * cases where there might be a race with the previous use of newpage.
	 * This is much like races on refcount of oldpage: just don't BUG().
	 */
	if (unlikely(!trylock_page(newpage)))
		goto out_unlock;

	if (unlikely(!is_lru)) {
		rc = move_to_new_page(newpage, page, mode);
		goto out_unlock_both;
	}

	/*
	 * Corner case handling:
	 * 1. When a new swap-cache page is read into, it is added to the LRU
	 * and treated as swapcache but it has no rmap yet.
	 * Calling try_to_unmap() against a page->mapping==NULL page will
	 * trigger a BUG.  So handle it here.
	 * 2. An orphaned page (see truncate_complete_page) might have
	 * fs-private metadata. The page can be picked up due to memory
	 * offlining.  Everywhere else except page reclaim, the page is
	 * invisible to the vm, so the page can not be migrated.  So try to
	 * free the metadata, so the page can be freed.
	 */
	if (!page->mapping) {
		VM_BUG_ON_PAGE(PageAnon(page), page);
		if (page_has_private(page)) {
			try_to_free_buffers(page);
			goto out_unlock_both;
		}
	} else if (page_mapped(page)) {
		/* Establish migration ptes */
		VM_BUG_ON_PAGE(PageAnon(page) && !PageKsm(page) && !anon_vma,
				page);
		try_to_unmap(page,
			TTU_MIGRATION|TTU_IGNORE_MLOCK|TTU_IGNORE_ACCESS);
		page_was_mapped = 1;
	}

	if (!page_mapped(page))
		rc = move_to_new_page(newpage, page, mode);

	if (page_was_mapped)
		remove_migration_ptes(page,
			rc == MIGRATEPAGE_SUCCESS ? newpage : page, false);

out_unlock_both:
	unlock_page(newpage);
out_unlock:
	/* Drop an anon_vma reference if we took one */
	if (anon_vma)
		put_anon_vma(anon_vma);
	unlock_page(page);
out:
	/*
	 * If migration is successful, decrease refcount of the newpage
	 * which will not free the page because new page owner increased
	 * refcounter. As well, if it is LRU page, add the page to LRU
	 * list in here.
	 */
	if (rc == MIGRATEPAGE_SUCCESS) {
		if (unlikely(__PageMovable(newpage)))
			put_page(newpage);
		else
			putback_lru_page(newpage);
	}

	return rc;
}
