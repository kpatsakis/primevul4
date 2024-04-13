static int nfs_writepage_setup(struct nfs_open_context *ctx, struct page *page,
		unsigned int offset, unsigned int count)
{
	struct nfs_page	*req;

	req = nfs_setup_write_request(ctx, page, offset, count);
	if (IS_ERR(req))
		return PTR_ERR(req);
	/* Update file length */
	nfs_grow_file(page, offset, count);
	nfs_mark_uptodate(page, req->wb_pgbase, req->wb_bytes);
	nfs_mark_request_dirty(req);
	nfs_unlock_and_release_request(req);
	return 0;
}
