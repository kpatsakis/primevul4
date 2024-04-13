int nfs_writepage(struct page *page, struct writeback_control *wbc)
{
	int ret;

	ret = nfs_writepage_locked(page, wbc);
	unlock_page(page);
	return ret;
}
