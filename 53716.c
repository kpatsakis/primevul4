static int fuse_writepage_locked(struct page *page)
{
	struct address_space *mapping = page->mapping;
	struct inode *inode = mapping->host;
	struct fuse_conn *fc = get_fuse_conn(inode);
	struct fuse_inode *fi = get_fuse_inode(inode);
	struct fuse_req *req;
	struct page *tmp_page;
	int error = -ENOMEM;

	set_page_writeback(page);

	req = fuse_request_alloc_nofs(1);
	if (!req)
		goto err;

	/* writeback always goes to bg_queue */
	__set_bit(FR_BACKGROUND, &req->flags);
	tmp_page = alloc_page(GFP_NOFS | __GFP_HIGHMEM);
	if (!tmp_page)
		goto err_free;

	error = -EIO;
	req->ff = fuse_write_file_get(fc, fi);
	if (!req->ff)
		goto err_nofile;

	fuse_write_fill(req, req->ff, page_offset(page), 0);

	copy_highpage(tmp_page, page);
	req->misc.write.in.write_flags |= FUSE_WRITE_CACHE;
	req->misc.write.next = NULL;
	req->in.argpages = 1;
	req->num_pages = 1;
	req->pages[0] = tmp_page;
	req->page_descs[0].offset = 0;
	req->page_descs[0].length = PAGE_SIZE;
	req->end = fuse_writepage_end;
	req->inode = inode;

	inc_wb_stat(&inode_to_bdi(inode)->wb, WB_WRITEBACK);
	inc_zone_page_state(tmp_page, NR_WRITEBACK_TEMP);

	spin_lock(&fc->lock);
	list_add(&req->writepages_entry, &fi->writepages);
	list_add_tail(&req->list, &fi->queued_writes);
	fuse_flush_writepages(inode);
	spin_unlock(&fc->lock);

	end_page_writeback(page);

	return 0;

err_nofile:
	__free_page(tmp_page);
err_free:
	fuse_request_free(req);
err:
	end_page_writeback(page);
	return error;
}
