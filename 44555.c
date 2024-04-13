static int nfs_page_async_flush(struct nfs_pageio_descriptor *pgio,
				struct page *page, bool nonblock)
{
	struct nfs_page *req;
	int ret = 0;

	req = nfs_find_and_lock_request(page, nonblock);
	if (!req)
		goto out;
	ret = PTR_ERR(req);
	if (IS_ERR(req))
		goto out;

	nfs_set_page_writeback(page);
	WARN_ON_ONCE(test_bit(PG_CLEAN, &req->wb_flags));

	ret = 0;
	if (!nfs_pageio_add_request(pgio, req)) {
		nfs_redirty_request(req);
		ret = pgio->pg_error;
	}
out:
	return ret;
}
