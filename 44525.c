struct nfs_commit_data *nfs_commitdata_alloc(void)
{
	struct nfs_commit_data *p = mempool_alloc(nfs_commit_mempool, GFP_NOIO);

	if (p) {
		memset(p, 0, sizeof(*p));
		INIT_LIST_HEAD(&p->pages);
	}
	return p;
}
