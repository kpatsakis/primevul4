struct nfs_write_header *nfs_writehdr_alloc(void)
{
	struct nfs_write_header *p = mempool_alloc(nfs_wdata_mempool, GFP_NOIO);

	if (p) {
		struct nfs_pgio_header *hdr = &p->header;

		memset(p, 0, sizeof(*p));
		INIT_LIST_HEAD(&hdr->pages);
		INIT_LIST_HEAD(&hdr->rpc_list);
		spin_lock_init(&hdr->lock);
		atomic_set(&hdr->refcnt, 0);
		hdr->verf = &p->verf;
	}
	return p;
}
