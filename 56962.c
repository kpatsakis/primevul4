static void xen_blkbk_unmap(struct xen_blkif_ring *ring,
                            struct grant_page *pages[],
                            int num)
{
	struct gnttab_unmap_grant_ref unmap[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct page *unmap_pages[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	unsigned int invcount = 0;
	int ret;

	while (num) {
		unsigned int batch = min(num, BLKIF_MAX_SEGMENTS_PER_REQUEST);

		invcount = xen_blkbk_unmap_prepare(ring, pages, batch,
						   unmap, unmap_pages);
		if (invcount) {
			ret = gnttab_unmap_refs(unmap, NULL, unmap_pages, invcount);
			BUG_ON(ret);
			put_free_pages(ring, unmap_pages, invcount);
		}
		pages += batch;
		num -= batch;
	}
}
