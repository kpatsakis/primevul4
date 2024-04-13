static int fuse_do_readpage(struct file *file, struct page *page)
{
	struct fuse_io_priv io = { .async = 0, .file = file };
	struct inode *inode = page->mapping->host;
	struct fuse_conn *fc = get_fuse_conn(inode);
	struct fuse_req *req;
	size_t num_read;
	loff_t pos = page_offset(page);
	size_t count = PAGE_CACHE_SIZE;
	u64 attr_ver;
	int err;

	/*
	 * Page writeback can extend beyond the lifetime of the
	 * page-cache page, so make sure we read a properly synced
	 * page.
	 */
	fuse_wait_on_page_writeback(inode, page->index);

	req = fuse_get_req(fc, 1);
	if (IS_ERR(req))
		return PTR_ERR(req);

	attr_ver = fuse_get_attr_version(fc);

	req->out.page_zeroing = 1;
	req->out.argpages = 1;
	req->num_pages = 1;
	req->pages[0] = page;
	req->page_descs[0].length = count;
	num_read = fuse_send_read(req, &io, pos, count, NULL);
	err = req->out.h.error;

	if (!err) {
		/*
		 * Short read means EOF.  If file size is larger, truncate it
		 */
		if (num_read < count)
			fuse_short_read(req, inode, attr_ver);

		SetPageUptodate(page);
	}

	fuse_put_request(fc, req);

	return err;
}
