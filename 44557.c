nfs_page_find_request_locked(struct nfs_inode *nfsi, struct page *page)
{
	struct nfs_page *req = NULL;

	if (PagePrivate(page))
		req = (struct nfs_page *)page_private(page);
	else if (unlikely(PageSwapCache(page))) {
		struct nfs_page *freq, *t;

		/* Linearly search the commit list for the correct req */
		list_for_each_entry_safe(freq, t, &nfsi->commit_info.list, wb_list) {
			if (freq->wb_page == page) {
				req = freq;
				break;
			}
		}
	}

	if (req)
		kref_get(&req->wb_kref);

	return req;
}
