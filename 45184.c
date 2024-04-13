cifs_uncached_writedata_release(struct kref *refcount)
{
	int i;
	struct cifs_writedata *wdata = container_of(refcount,
					struct cifs_writedata, refcount);

	for (i = 0; i < wdata->nr_pages; i++)
		put_page(wdata->pages[i]);
	cifs_writedata_release(refcount);
}
