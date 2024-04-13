cifs_readv_complete(struct work_struct *work)
{
	unsigned int i;
	struct cifs_readdata *rdata = container_of(work,
						struct cifs_readdata, work);

	for (i = 0; i < rdata->nr_pages; i++) {
		struct page *page = rdata->pages[i];

		lru_cache_add_file(page);

		if (rdata->result == 0) {
			flush_dcache_page(page);
			SetPageUptodate(page);
		}

		unlock_page(page);

		if (rdata->result == 0)
			cifs_readpage_to_fscache(rdata->mapping->host, page);

		page_cache_release(page);
		rdata->pages[i] = NULL;
	}
	kref_put(&rdata->refcount, cifs_readdata_release);
}
