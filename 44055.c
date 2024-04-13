get_futex_key(u32 __user *uaddr, int fshared, union futex_key *key, int rw)
{
	unsigned long address = (unsigned long)uaddr;
	struct mm_struct *mm = current->mm;
	struct page *page, *page_head;
	int err, ro = 0;

	/*
	 * The futex address must be "naturally" aligned.
	 */
	key->both.offset = address % PAGE_SIZE;
	if (unlikely((address % sizeof(u32)) != 0))
		return -EINVAL;
	address -= key->both.offset;

	if (unlikely(!access_ok(rw, uaddr, sizeof(u32))))
		return -EFAULT;

	/*
	 * PROCESS_PRIVATE futexes are fast.
	 * As the mm cannot disappear under us and the 'key' only needs
	 * virtual address, we dont even have to find the underlying vma.
	 * Note : We do have to check 'uaddr' is a valid user address,
	 *        but access_ok() should be faster than find_vma()
	 */
	if (!fshared) {
		key->private.mm = mm;
		key->private.address = address;
		get_futex_key_refs(key);  /* implies MB (B) */
		return 0;
	}

again:
	err = get_user_pages_fast(address, 1, 1, &page);
	/*
	 * If write access is not required (eg. FUTEX_WAIT), try
	 * and get read-only access.
	 */
	if (err == -EFAULT && rw == VERIFY_READ) {
		err = get_user_pages_fast(address, 1, 0, &page);
		ro = 1;
	}
	if (err < 0)
		return err;
	else
		err = 0;

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
	page_head = page;
	if (unlikely(PageTail(page))) {
		put_page(page);
		/* serialize against __split_huge_page_splitting() */
		local_irq_disable();
		if (likely(__get_user_pages_fast(address, 1, !ro, &page) == 1)) {
			page_head = compound_head(page);
			/*
			 * page_head is valid pointer but we must pin
			 * it before taking the PG_lock and/or
			 * PG_compound_lock. The moment we re-enable
			 * irqs __split_huge_page_splitting() can
			 * return and the head page can be freed from
			 * under us. We can't take the PG_lock and/or
			 * PG_compound_lock on a page that could be
			 * freed from under us.
			 */
			if (page != page_head) {
				get_page(page_head);
				put_page(page);
			}
			local_irq_enable();
		} else {
			local_irq_enable();
			goto again;
		}
	}
#else
	page_head = compound_head(page);
	if (page != page_head) {
		get_page(page_head);
		put_page(page);
	}
#endif

	lock_page(page_head);

	/*
	 * If page_head->mapping is NULL, then it cannot be a PageAnon
	 * page; but it might be the ZERO_PAGE or in the gate area or
	 * in a special mapping (all cases which we are happy to fail);
	 * or it may have been a good file page when get_user_pages_fast
	 * found it, but truncated or holepunched or subjected to
	 * invalidate_complete_page2 before we got the page lock (also
	 * cases which we are happy to fail).  And we hold a reference,
	 * so refcount care in invalidate_complete_page's remove_mapping
	 * prevents drop_caches from setting mapping to NULL beneath us.
	 *
	 * The case we do have to guard against is when memory pressure made
	 * shmem_writepage move it from filecache to swapcache beneath us:
	 * an unlikely race, but we do need to retry for page_head->mapping.
	 */
	if (!page_head->mapping) {
		int shmem_swizzled = PageSwapCache(page_head);
		unlock_page(page_head);
		put_page(page_head);
		if (shmem_swizzled)
			goto again;
		return -EFAULT;
	}

	/*
	 * Private mappings are handled in a simple way.
	 *
	 * NOTE: When userspace waits on a MAP_SHARED mapping, even if
	 * it's a read-only handle, it's expected that futexes attach to
	 * the object not the particular process.
	 */
	if (PageAnon(page_head)) {
		/*
		 * A RO anonymous page will never change and thus doesn't make
		 * sense for futex operations.
		 */
		if (ro) {
			err = -EFAULT;
			goto out;
		}

		key->both.offset |= FUT_OFF_MMSHARED; /* ref taken on mm */
		key->private.mm = mm;
		key->private.address = address;
	} else {
		key->both.offset |= FUT_OFF_INODE; /* inode-based key */
		key->shared.inode = page_head->mapping->host;
		key->shared.pgoff = basepage_index(page);
	}

	get_futex_key_refs(key); /* implies MB (B) */

out:
	unlock_page(page_head);
	put_page(page_head);
	return err;
}
