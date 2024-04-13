static int do_truncation(struct ubifs_info *c, struct inode *inode,
			 const struct iattr *attr)
{
	int err;
	struct ubifs_budget_req req;
	loff_t old_size = inode->i_size, new_size = attr->ia_size;
	int offset = new_size & (UBIFS_BLOCK_SIZE - 1), budgeted = 1;
	struct ubifs_inode *ui = ubifs_inode(inode);

	dbg_gen("ino %lu, size %lld -> %lld", inode->i_ino, old_size, new_size);
	memset(&req, 0, sizeof(struct ubifs_budget_req));

	/*
	 * If this is truncation to a smaller size, and we do not truncate on a
	 * block boundary, budget for changing one data block, because the last
	 * block will be re-written.
	 */
	if (new_size & (UBIFS_BLOCK_SIZE - 1))
		req.dirtied_page = 1;

	req.dirtied_ino = 1;
	/* A funny way to budget for truncation node */
	req.dirtied_ino_d = UBIFS_TRUN_NODE_SZ;
	err = ubifs_budget_space(c, &req);
	if (err) {
		/*
		 * Treat truncations to zero as deletion and always allow them,
		 * just like we do for '->unlink()'.
		 */
		if (new_size || err != -ENOSPC)
			return err;
		budgeted = 0;
	}

	truncate_setsize(inode, new_size);

	if (offset) {
		pgoff_t index = new_size >> PAGE_CACHE_SHIFT;
		struct page *page;

		page = find_lock_page(inode->i_mapping, index);
		if (page) {
			if (PageDirty(page)) {
				/*
				 * 'ubifs_jnl_truncate()' will try to truncate
				 * the last data node, but it contains
				 * out-of-date data because the page is dirty.
				 * Write the page now, so that
				 * 'ubifs_jnl_truncate()' will see an already
				 * truncated (and up to date) data node.
				 */
				ubifs_assert(PagePrivate(page));

				clear_page_dirty_for_io(page);
				if (UBIFS_BLOCKS_PER_PAGE_SHIFT)
					offset = new_size &
						 (PAGE_CACHE_SIZE - 1);
				err = do_writepage(page, offset);
				page_cache_release(page);
				if (err)
					goto out_budg;
				/*
				 * We could now tell 'ubifs_jnl_truncate()' not
				 * to read the last block.
				 */
			} else {
				/*
				 * We could 'kmap()' the page and pass the data
				 * to 'ubifs_jnl_truncate()' to save it from
				 * having to read it.
				 */
				unlock_page(page);
				page_cache_release(page);
			}
		}
	}

	mutex_lock(&ui->ui_mutex);
	ui->ui_size = inode->i_size;
	/* Truncation changes inode [mc]time */
	inode->i_mtime = inode->i_ctime = ubifs_current_time(inode);
	/* Other attributes may be changed at the same time as well */
	do_attr_changes(inode, attr);
	err = ubifs_jnl_truncate(c, inode, old_size, new_size);
	mutex_unlock(&ui->ui_mutex);

out_budg:
	if (budgeted)
		ubifs_release_budget(c, &req);
	else {
		c->bi.nospace = c->bi.nospace_rp = 0;
		smp_wmb();
	}
	return err;
}
