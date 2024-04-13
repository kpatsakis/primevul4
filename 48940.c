static int write_begin_slow(struct address_space *mapping,
			    loff_t pos, unsigned len, struct page **pagep,
			    unsigned flags)
{
	struct inode *inode = mapping->host;
	struct ubifs_info *c = inode->i_sb->s_fs_info;
	pgoff_t index = pos >> PAGE_CACHE_SHIFT;
	struct ubifs_budget_req req = { .new_page = 1 };
	int uninitialized_var(err), appending = !!(pos + len > inode->i_size);
	struct page *page;

	dbg_gen("ino %lu, pos %llu, len %u, i_size %lld",
		inode->i_ino, pos, len, inode->i_size);

	/*
	 * At the slow path we have to budget before locking the page, because
	 * budgeting may force write-back, which would wait on locked pages and
	 * deadlock if we had the page locked. At this point we do not know
	 * anything about the page, so assume that this is a new page which is
	 * written to a hole. This corresponds to largest budget. Later the
	 * budget will be amended if this is not true.
	 */
	if (appending)
		/* We are appending data, budget for inode change */
		req.dirtied_ino = 1;

	err = ubifs_budget_space(c, &req);
	if (unlikely(err))
		return err;

	page = grab_cache_page_write_begin(mapping, index, flags);
	if (unlikely(!page)) {
		ubifs_release_budget(c, &req);
		return -ENOMEM;
	}

	if (!PageUptodate(page)) {
		if (!(pos & ~PAGE_CACHE_MASK) && len == PAGE_CACHE_SIZE)
			SetPageChecked(page);
		else {
			err = do_readpage(page);
			if (err) {
				unlock_page(page);
				page_cache_release(page);
				return err;
			}
		}

		SetPageUptodate(page);
		ClearPageError(page);
	}

	if (PagePrivate(page))
		/*
		 * The page is dirty, which means it was budgeted twice:
		 *   o first time the budget was allocated by the task which
		 *     made the page dirty and set the PG_private flag;
		 *   o and then we budgeted for it for the second time at the
		 *     very beginning of this function.
		 *
		 * So what we have to do is to release the page budget we
		 * allocated.
		 */
		release_new_page_budget(c);
	else if (!PageChecked(page))
		/*
		 * We are changing a page which already exists on the media.
		 * This means that changing the page does not make the amount
		 * of indexing information larger, and this part of the budget
		 * which we have already acquired may be released.
		 */
		ubifs_convert_page_budget(c);

	if (appending) {
		struct ubifs_inode *ui = ubifs_inode(inode);

		/*
		 * 'ubifs_write_end()' is optimized from the fast-path part of
		 * 'ubifs_write_begin()' and expects the @ui_mutex to be locked
		 * if data is appended.
		 */
		mutex_lock(&ui->ui_mutex);
		if (ui->dirty)
			/*
			 * The inode is dirty already, so we may free the
			 * budget we allocated.
			 */
			ubifs_release_dirty_inode_budget(c, ui);
	}

	*pagep = page;
	return 0;
}
