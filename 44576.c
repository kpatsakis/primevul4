int nfs_wb_page_cancel(struct inode *inode, struct page *page)
{
	struct nfs_page *req;
	int ret = 0;

	for (;;) {
		wait_on_page_writeback(page);
		req = nfs_page_find_request(page);
		if (req == NULL)
			break;
		if (nfs_lock_request(req)) {
			nfs_clear_request_commit(req);
			nfs_inode_remove_request(req);
			/*
			 * In case nfs_inode_remove_request has marked the
			 * page as being dirty
			 */
			cancel_dirty_page(page, PAGE_CACHE_SIZE);
			nfs_unlock_and_release_request(req);
			break;
		}
		ret = nfs_wait_on_request(req);
		nfs_release_request(req);
		if (ret < 0)
			break;
	}
	return ret;
}
