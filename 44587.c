void nfs_writedata_release(struct nfs_write_data *wdata)
{
	struct nfs_pgio_header *hdr = wdata->header;
	struct nfs_write_header *write_header = container_of(hdr, struct nfs_write_header, header);

	put_nfs_open_context(wdata->args.context);
	if (wdata->pages.pagevec != wdata->pages.page_array)
		kfree(wdata->pages.pagevec);
	if (wdata == &write_header->rpc_data) {
		wdata->header = NULL;
		wdata = NULL;
	}
	if (atomic_dec_and_test(&hdr->refcnt))
		hdr->completion_ops->completion(hdr);
	/* Note: we only free the rpc_task after callbacks are done.
	 * See the comment in rpc_free_task() for why
	 */
	kfree(wdata);
}
