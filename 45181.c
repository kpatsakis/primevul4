cifs_uncached_readdata_release(struct kref *refcount)
{
	struct cifs_readdata *rdata = container_of(refcount,
					struct cifs_readdata, refcount);
	unsigned int i;

	for (i = 0; i < rdata->nr_pages; i++) {
		put_page(rdata->pages[i]);
		rdata->pages[i] = NULL;
	}
	cifs_readdata_release(refcount);
}
