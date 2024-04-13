static int nfs_flush_multi(struct nfs_pageio_descriptor *desc,
			   struct nfs_pgio_header *hdr)
{
	struct nfs_page *req = hdr->req;
	struct page *page = req->wb_page;
	struct nfs_write_data *data;
	size_t wsize = desc->pg_bsize, nbytes;
	unsigned int offset;
	int requests = 0;
	struct nfs_commit_info cinfo;

	nfs_init_cinfo(&cinfo, desc->pg_inode, desc->pg_dreq);

	if ((desc->pg_ioflags & FLUSH_COND_STABLE) &&
	    (desc->pg_moreio || nfs_reqs_to_commit(&cinfo) ||
	     desc->pg_count > wsize))
		desc->pg_ioflags &= ~FLUSH_COND_STABLE;


	offset = 0;
	nbytes = desc->pg_count;
	do {
		size_t len = min(nbytes, wsize);

		data = nfs_writedata_alloc(hdr, 1);
		if (!data) {
			nfs_flush_error(desc, hdr);
			return -ENOMEM;
		}
		data->pages.pagevec[0] = page;
		nfs_write_rpcsetup(data, len, offset, desc->pg_ioflags, &cinfo);
		list_add(&data->list, &hdr->rpc_list);
		requests++;
		nbytes -= len;
		offset += len;
	} while (nbytes != 0);
	nfs_list_remove_request(req);
	nfs_list_add_request(req, &hdr->pages);
	desc->pg_rpc_callops = &nfs_write_common_ops;
	return 0;
}
