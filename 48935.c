static int ubifs_write_begin(struct file *file, struct address_space *mapping,
			     loff_t pos, unsigned len, unsigned flags,
			     struct page **pagep, void **fsdata)
{
	struct inode *inode = mapping->host;
	struct ubifs_info *c = inode->i_sb->s_fs_info;
	struct ubifs_inode *ui = ubifs_inode(inode);
	pgoff_t index = pos >> PAGE_CACHE_SHIFT;
	int uninitialized_var(err), appending = !!(pos + len > inode->i_size);
	int skipped_read = 0;
	struct page *page;

	ubifs_assert(ubifs_inode(inode)->ui_size == inode->i_size);
	ubifs_assert(!c->ro_media && !c->ro_mount);

	if (unlikely(c->ro_error))
		return -EROFS;

	/* Try out the fast-path part first */
	page = grab_cache_page_write_begin(mapping, index, flags);
	if (unlikely(!page))
		return -ENOMEM;

	if (!PageUptodate(page)) {
		/* The page is not loaded from the flash */
		if (!(pos & ~PAGE_CACHE_MASK) && len == PAGE_CACHE_SIZE) {
			/*
			 * We change whole page so no need to load it. But we
			 * do not know whether this page exists on the media or
			 * not, so we assume the latter because it requires
			 * larger budget. The assumption is that it is better
			 * to budget a bit more than to read the page from the
			 * media. Thus, we are setting the @PG_checked flag
			 * here.
			 */
			SetPageChecked(page);
			skipped_read = 1;
		} else {
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

	err = allocate_budget(c, page, ui, appending);
	if (unlikely(err)) {
		ubifs_assert(err == -ENOSPC);
		/*
		 * If we skipped reading the page because we were going to
		 * write all of it, then it is not up to date.
		 */
		if (skipped_read) {
			ClearPageChecked(page);
			ClearPageUptodate(page);
		}
		/*
		 * Budgeting failed which means it would have to force
		 * write-back but didn't, because we set the @fast flag in the
		 * request. Write-back cannot be done now, while we have the
		 * page locked, because it would deadlock. Unlock and free
		 * everything and fall-back to slow-path.
		 */
		if (appending) {
			ubifs_assert(mutex_is_locked(&ui->ui_mutex));
			mutex_unlock(&ui->ui_mutex);
		}
		unlock_page(page);
		page_cache_release(page);

		return write_begin_slow(mapping, pos, len, pagep, flags);
	}

	/*
	 * Whee, we acquired budgeting quickly - without involving
	 * garbage-collection, committing or forcing write-back. We return
	 * with @ui->ui_mutex locked if we are appending pages, and unlocked
	 * otherwise. This is an optimization (slightly hacky though).
	 */
	*pagep = page;
	return 0;

}
