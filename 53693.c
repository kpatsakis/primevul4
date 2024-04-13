static int fuse_readpages_fill(void *_data, struct page *page)
{
	struct fuse_fill_data *data = _data;
	struct fuse_req *req = data->req;
	struct inode *inode = data->inode;
	struct fuse_conn *fc = get_fuse_conn(inode);

	fuse_wait_on_page_writeback(inode, page->index);

	if (req->num_pages &&
	    (req->num_pages == FUSE_MAX_PAGES_PER_REQ ||
	     (req->num_pages + 1) * PAGE_CACHE_SIZE > fc->max_read ||
	     req->pages[req->num_pages - 1]->index + 1 != page->index)) {
		int nr_alloc = min_t(unsigned, data->nr_pages,
				     FUSE_MAX_PAGES_PER_REQ);
		fuse_send_readpages(req, data->file);
		if (fc->async_read)
			req = fuse_get_req_for_background(fc, nr_alloc);
		else
			req = fuse_get_req(fc, nr_alloc);

		data->req = req;
		if (IS_ERR(req)) {
			unlock_page(page);
			return PTR_ERR(req);
		}
	}

	if (WARN_ON(req->num_pages >= req->max_pages)) {
		fuse_put_request(fc, req);
		return -EIO;
	}

	page_cache_get(page);
	req->pages[req->num_pages] = page;
	req->page_descs[req->num_pages].length = PAGE_SIZE;
	req->num_pages++;
	data->nr_pages--;
	return 0;
}
