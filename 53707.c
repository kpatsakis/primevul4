static int fuse_write_end(struct file *file, struct address_space *mapping,
		loff_t pos, unsigned len, unsigned copied,
		struct page *page, void *fsdata)
{
	struct inode *inode = page->mapping->host;

	if (!PageUptodate(page)) {
		/* Zero any unwritten bytes at the end of the page */
		size_t endoff = (pos + copied) & ~PAGE_CACHE_MASK;
		if (endoff)
			zero_user_segment(page, endoff, PAGE_CACHE_SIZE);
		SetPageUptodate(page);
	}

	fuse_write_update_size(inode, pos + copied);
	set_page_dirty(page);
	unlock_page(page);
	page_cache_release(page);

	return copied;
}
