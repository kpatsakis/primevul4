static int ubifs_vm_page_mkwrite(struct vm_area_struct *vma,
				 struct vm_fault *vmf)
{
	struct page *page = vmf->page;
	struct inode *inode = file_inode(vma->vm_file);
	struct ubifs_info *c = inode->i_sb->s_fs_info;
	struct timespec now = ubifs_current_time(inode);
	struct ubifs_budget_req req = { .new_page = 1 };
	int err, update_time;

	dbg_gen("ino %lu, pg %lu, i_size %lld",	inode->i_ino, page->index,
		i_size_read(inode));
	ubifs_assert(!c->ro_media && !c->ro_mount);

	if (unlikely(c->ro_error))
		return VM_FAULT_SIGBUS; /* -EROFS */

	/*
	 * We have not locked @page so far so we may budget for changing the
	 * page. Note, we cannot do this after we locked the page, because
	 * budgeting may cause write-back which would cause deadlock.
	 *
	 * At the moment we do not know whether the page is dirty or not, so we
	 * assume that it is not and budget for a new page. We could look at
	 * the @PG_private flag and figure this out, but we may race with write
	 * back and the page state may change by the time we lock it, so this
	 * would need additional care. We do not bother with this at the
	 * moment, although it might be good idea to do. Instead, we allocate
	 * budget for a new page and amend it later on if the page was in fact
	 * dirty.
	 *
	 * The budgeting-related logic of this function is similar to what we
	 * do in 'ubifs_write_begin()' and 'ubifs_write_end()'. Glance there
	 * for more comments.
	 */
	update_time = mctime_update_needed(inode, &now);
	if (update_time)
		/*
		 * We have to change inode time stamp which requires extra
		 * budgeting.
		 */
		req.dirtied_ino = 1;

	err = ubifs_budget_space(c, &req);
	if (unlikely(err)) {
		if (err == -ENOSPC)
			ubifs_warn("out of space for mmapped file (inode number %lu)",
				   inode->i_ino);
		return VM_FAULT_SIGBUS;
	}

	lock_page(page);
	if (unlikely(page->mapping != inode->i_mapping ||
		     page_offset(page) > i_size_read(inode))) {
		/* Page got truncated out from underneath us */
		err = -EINVAL;
		goto out_unlock;
	}

	if (PagePrivate(page))
		release_new_page_budget(c);
	else {
		if (!PageChecked(page))
			ubifs_convert_page_budget(c);
		SetPagePrivate(page);
		atomic_long_inc(&c->dirty_pg_cnt);
		__set_page_dirty_nobuffers(page);
	}

	if (update_time) {
		int release;
		struct ubifs_inode *ui = ubifs_inode(inode);

		mutex_lock(&ui->ui_mutex);
		inode->i_mtime = inode->i_ctime = ubifs_current_time(inode);
		release = ui->dirty;
		mark_inode_dirty_sync(inode);
		mutex_unlock(&ui->ui_mutex);
		if (release)
			ubifs_release_dirty_inode_budget(c, ui);
	}

	wait_for_stable_page(page);
	unlock_page(page);
	return 0;

out_unlock:
	unlock_page(page);
	ubifs_release_budget(c, &req);
	if (err)
		err = VM_FAULT_SIGBUS;
	return err;
}
