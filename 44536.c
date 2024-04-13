static int nfs_flush_one(struct nfs_pageio_descriptor *desc,
			 struct nfs_pgio_header *hdr)
{
	struct nfs_page		*req;
	struct page		**pages;
	struct nfs_write_data	*data;
	struct list_head *head = &desc->pg_list;
	struct nfs_commit_info cinfo;

	data = nfs_writedata_alloc(hdr, nfs_page_array_len(desc->pg_base,
							   desc->pg_count));
	if (!data) {
		nfs_flush_error(desc, hdr);
		return -ENOMEM;
	}

	nfs_init_cinfo(&cinfo, desc->pg_inode, desc->pg_dreq);
	pages = data->pages.pagevec;
	while (!list_empty(head)) {
		req = nfs_list_entry(head->next);
		nfs_list_remove_request(req);
		nfs_list_add_request(req, &hdr->pages);
		*pages++ = req->wb_page;
	}

	if ((desc->pg_ioflags & FLUSH_COND_STABLE) &&
	    (desc->pg_moreio || nfs_reqs_to_commit(&cinfo)))
		desc->pg_ioflags &= ~FLUSH_COND_STABLE;

	/* Set up the argument struct */
	nfs_write_rpcsetup(data, desc->pg_count, 0, desc->pg_ioflags, &cinfo);
	list_add(&data->list, &hdr->rpc_list);
	desc->pg_rpc_callops = &nfs_write_common_ops;
	return 0;
}
