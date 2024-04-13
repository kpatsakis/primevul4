static void fuse_short_read(struct fuse_req *req, struct inode *inode,
			    u64 attr_ver)
{
	size_t num_read = req->out.args[0].size;
	struct fuse_conn *fc = get_fuse_conn(inode);

	if (fc->writeback_cache) {
		/*
		 * A hole in a file. Some data after the hole are in page cache,
		 * but have not reached the client fs yet. So, the hole is not
		 * present there.
		 */
		int i;
		int start_idx = num_read >> PAGE_CACHE_SHIFT;
		size_t off = num_read & (PAGE_CACHE_SIZE - 1);

		for (i = start_idx; i < req->num_pages; i++) {
			zero_user_segment(req->pages[i], off, PAGE_CACHE_SIZE);
			off = 0;
		}
	} else {
		loff_t pos = page_offset(req->pages[0]) + num_read;
		fuse_read_update_size(inode, pos, attr_ver);
	}
}
