shmem_write_end(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned copied,
			struct page *page, void *fsdata)
{
	struct inode *inode = mapping->host;

	if (pos + copied > inode->i_size)
		i_size_write(inode, pos + copied);

	if (!PageUptodate(page)) {
		if (copied < PAGE_CACHE_SIZE) {
			unsigned from = pos & (PAGE_CACHE_SIZE - 1);
			zero_user_segments(page, 0, from,
					from + copied, PAGE_CACHE_SIZE);
		}
		SetPageUptodate(page);
	}
	set_page_dirty(page);
	unlock_page(page);
	page_cache_release(page);

	return copied;
}
