static bool fuse_range_is_writeback(struct inode *inode, pgoff_t idx_from,
				   pgoff_t idx_to)
{
	struct fuse_conn *fc = get_fuse_conn(inode);
	struct fuse_inode *fi = get_fuse_inode(inode);
	struct fuse_req *req;
	bool found = false;

	spin_lock(&fc->lock);
	list_for_each_entry(req, &fi->writepages, writepages_entry) {
		pgoff_t curr_index;

		BUG_ON(req->inode != inode);
		curr_index = req->misc.write.in.offset >> PAGE_CACHE_SHIFT;
		if (idx_from < curr_index + req->num_pages &&
		    curr_index <= idx_to) {
			found = true;
			break;
		}
	}
	spin_unlock(&fc->lock);

	return found;
}
