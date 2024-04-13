static int nfs_writepage_locked(struct page *page, struct writeback_control *wbc)
{
	struct nfs_pageio_descriptor pgio;
	int err;

	NFS_PROTO(page_file_mapping(page)->host)->write_pageio_init(&pgio,
							  page->mapping->host,
							  wb_priority(wbc),
							  &nfs_async_write_completion_ops);
	err = nfs_do_writepage(page, wbc, &pgio);
	nfs_pageio_complete(&pgio);
	if (err < 0)
		return err;
	if (pgio.pg_error < 0)
		return pgio.pg_error;
	return 0;
}
