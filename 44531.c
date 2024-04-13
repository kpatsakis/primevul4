static int nfs_do_writepage(struct page *page, struct writeback_control *wbc, struct nfs_pageio_descriptor *pgio)
{
	struct inode *inode = page_file_mapping(page)->host;
	int ret;

	nfs_inc_stats(inode, NFSIOS_VFSWRITEPAGE);
	nfs_add_stats(inode, NFSIOS_WRITEPAGES, 1);

	nfs_pageio_cond_complete(pgio, page_file_index(page));
	ret = nfs_page_async_flush(pgio, page, wbc->sync_mode == WB_SYNC_NONE);
	if (ret == -EAGAIN) {
		redirty_page_for_writepage(wbc, page);
		ret = 0;
	}
	return ret;
}
