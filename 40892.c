static void unmap_purged_grants(struct work_struct *work)
{
	struct gnttab_unmap_grant_ref unmap[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct page *pages[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct persistent_gnt *persistent_gnt;
	int ret, segs_to_unmap = 0;
	struct xen_blkif *blkif = container_of(work, typeof(*blkif), persistent_purge_work);

	while(!list_empty(&blkif->persistent_purge_list)) {
		persistent_gnt = list_first_entry(&blkif->persistent_purge_list,
		                                  struct persistent_gnt,
		                                  remove_node);
		list_del(&persistent_gnt->remove_node);

		gnttab_set_unmap_op(&unmap[segs_to_unmap],
			vaddr(persistent_gnt->page),
			GNTMAP_host_map,
			persistent_gnt->handle);

		pages[segs_to_unmap] = persistent_gnt->page;

		if (++segs_to_unmap == BLKIF_MAX_SEGMENTS_PER_REQUEST) {
			ret = gnttab_unmap_refs(unmap, NULL, pages,
				segs_to_unmap);
			BUG_ON(ret);
			put_free_pages(blkif, pages, segs_to_unmap);
			segs_to_unmap = 0;
		}
		kfree(persistent_gnt);
	}
	if (segs_to_unmap > 0) {
		ret = gnttab_unmap_refs(unmap, NULL, pages, segs_to_unmap);
		BUG_ON(ret);
		put_free_pages(blkif, pages, segs_to_unmap);
	}
}
