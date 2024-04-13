static bool fuse_writepage_in_flight(struct fuse_req *new_req,
				     struct page *page)
{
	struct fuse_conn *fc = get_fuse_conn(new_req->inode);
	struct fuse_inode *fi = get_fuse_inode(new_req->inode);
	struct fuse_req *tmp;
	struct fuse_req *old_req;
	bool found = false;
	pgoff_t curr_index;

	BUG_ON(new_req->num_pages != 0);

	spin_lock(&fc->lock);
	list_del(&new_req->writepages_entry);
	list_for_each_entry(old_req, &fi->writepages, writepages_entry) {
		BUG_ON(old_req->inode != new_req->inode);
		curr_index = old_req->misc.write.in.offset >> PAGE_CACHE_SHIFT;
		if (curr_index <= page->index &&
		    page->index < curr_index + old_req->num_pages) {
			found = true;
			break;
		}
	}
	if (!found) {
		list_add(&new_req->writepages_entry, &fi->writepages);
		goto out_unlock;
	}

	new_req->num_pages = 1;
	for (tmp = old_req; tmp != NULL; tmp = tmp->misc.write.next) {
		BUG_ON(tmp->inode != new_req->inode);
		curr_index = tmp->misc.write.in.offset >> PAGE_CACHE_SHIFT;
		if (tmp->num_pages == 1 &&
		    curr_index == page->index) {
			old_req = tmp;
		}
	}

	if (old_req->num_pages == 1 && test_bit(FR_PENDING, &old_req->flags)) {
		struct backing_dev_info *bdi = inode_to_bdi(page->mapping->host);

		copy_highpage(old_req->pages[0], page);
		spin_unlock(&fc->lock);

		dec_wb_stat(&bdi->wb, WB_WRITEBACK);
		dec_zone_page_state(page, NR_WRITEBACK_TEMP);
		wb_writeout_inc(&bdi->wb);
		fuse_writepage_free(fc, new_req);
		fuse_request_free(new_req);
		goto out;
	} else {
		new_req->misc.write.next = old_req->misc.write.next;
		old_req->misc.write.next = new_req;
	}
out_unlock:
	spin_unlock(&fc->lock);
out:
	return found;
}
