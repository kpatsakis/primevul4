static void truncate_partial_data_page(struct inode *inode, u64 from)
{
	unsigned offset = from & (PAGE_CACHE_SIZE - 1);
	struct page *page;

	if (!offset)
		return;

	page = find_data_page(inode, from >> PAGE_CACHE_SHIFT, false);
	if (IS_ERR(page))
		return;

	lock_page(page);
	if (unlikely(page->mapping != inode->i_mapping)) {
		f2fs_put_page(page, 1);
		return;
	}
	f2fs_wait_on_page_writeback(page, DATA);
	zero_user(page, offset, PAGE_CACHE_SIZE - offset);
	set_page_dirty(page);
	f2fs_put_page(page, 1);
}
