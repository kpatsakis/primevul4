int nfs_flush_incompatible(struct file *file, struct page *page)
{
	struct nfs_open_context *ctx = nfs_file_open_context(file);
	struct nfs_lock_context *l_ctx;
	struct nfs_page	*req;
	int do_flush, status;
	/*
	 * Look for a request corresponding to this page. If there
	 * is one, and it belongs to another file, we flush it out
	 * before we try to copy anything into the page. Do this
	 * due to the lack of an ACCESS-type call in NFSv2.
	 * Also do the same if we find a request from an existing
	 * dropped page.
	 */
	do {
		req = nfs_page_find_request(page);
		if (req == NULL)
			return 0;
		l_ctx = req->wb_lock_context;
		do_flush = req->wb_page != page || req->wb_context != ctx;
		if (l_ctx && ctx->dentry->d_inode->i_flock != NULL) {
			do_flush |= l_ctx->lockowner.l_owner != current->files
				|| l_ctx->lockowner.l_pid != current->tgid;
		}
		nfs_release_request(req);
		if (!do_flush)
			return 0;
		status = nfs_wb_page(page_file_mapping(page)->host, page);
	} while (status == 0);
	return status;
}
