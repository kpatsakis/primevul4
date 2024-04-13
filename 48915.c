static int allocate_budget(struct ubifs_info *c, struct page *page,
			   struct ubifs_inode *ui, int appending)
{
	struct ubifs_budget_req req = { .fast = 1 };

	if (PagePrivate(page)) {
		if (!appending)
			/*
			 * The page is dirty and we are not appending, which
			 * means no budget is needed at all.
			 */
			return 0;

		mutex_lock(&ui->ui_mutex);
		if (ui->dirty)
			/*
			 * The page is dirty and we are appending, so the inode
			 * has to be marked as dirty. However, it is already
			 * dirty, so we do not need any budget. We may return,
			 * but @ui->ui_mutex hast to be left locked because we
			 * should prevent write-back from flushing the inode
			 * and freeing the budget. The lock will be released in
			 * 'ubifs_write_end()'.
			 */
			return 0;

		/*
		 * The page is dirty, we are appending, the inode is clean, so
		 * we need to budget the inode change.
		 */
		req.dirtied_ino = 1;
	} else {
		if (PageChecked(page))
			/*
			 * The page corresponds to a hole and does not
			 * exist on the media. So changing it makes
			 * make the amount of indexing information
			 * larger, and we have to budget for a new
			 * page.
			 */
			req.new_page = 1;
		else
			/*
			 * Not a hole, the change will not add any new
			 * indexing information, budget for page
			 * change.
			 */
			req.dirtied_page = 1;

		if (appending) {
			mutex_lock(&ui->ui_mutex);
			if (!ui->dirty)
				/*
				 * The inode is clean but we will have to mark
				 * it as dirty because we are appending. This
				 * needs a budget.
				 */
				req.dirtied_ino = 1;
		}
	}

	return ubifs_budget_space(c, &req);
}
