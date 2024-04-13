static void xen_blkbk_unmap(struct xen_blkif *blkif,
                            struct grant_page *pages[],
                            int num)
{
	struct gnttab_unmap_grant_ref unmap[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct page *unmap_pages[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	unsigned int i, invcount = 0;
	int ret;

	for (i = 0; i < num; i++) {
		if (pages[i]->persistent_gnt != NULL) {
			put_persistent_gnt(blkif, pages[i]->persistent_gnt);
			continue;
		}
		if (pages[i]->handle == BLKBACK_INVALID_HANDLE)
			continue;
		unmap_pages[invcount] = pages[i]->page;
		gnttab_set_unmap_op(&unmap[invcount], vaddr(pages[i]->page),
				    GNTMAP_host_map, pages[i]->handle);
		pages[i]->handle = BLKBACK_INVALID_HANDLE;
		if (++invcount == BLKIF_MAX_SEGMENTS_PER_REQUEST) {
			ret = gnttab_unmap_refs(unmap, NULL, unmap_pages,
			                        invcount);
			BUG_ON(ret);
			put_free_pages(blkif, unmap_pages, invcount);
			invcount = 0;
		}
	}
	if (invcount) {
		ret = gnttab_unmap_refs(unmap, NULL, unmap_pages, invcount);
		BUG_ON(ret);
		put_free_pages(blkif, unmap_pages, invcount);
	}
}
