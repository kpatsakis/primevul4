static int fuse_writepages(struct address_space *mapping,
			   struct writeback_control *wbc)
{
	struct inode *inode = mapping->host;
	struct fuse_fill_wb_data data;
	int err;

	err = -EIO;
	if (is_bad_inode(inode))
		goto out;

	data.inode = inode;
	data.req = NULL;
	data.ff = NULL;

	err = -ENOMEM;
	data.orig_pages = kcalloc(FUSE_MAX_PAGES_PER_REQ,
				  sizeof(struct page *),
				  GFP_NOFS);
	if (!data.orig_pages)
		goto out;

	err = write_cache_pages(mapping, wbc, fuse_writepages_fill, &data);
	if (data.req) {
		/* Ignore errors if we can write at least one page */
		BUG_ON(!data.req->num_pages);
		fuse_writepages_send(&data);
		err = 0;
	}
	if (data.ff)
		fuse_file_put(data.ff, false);

	kfree(data.orig_pages);
out:
	return err;
}
