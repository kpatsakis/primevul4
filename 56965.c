void xen_blkbk_unmap_purged_grants(struct work_struct *work)
{
	struct gnttab_unmap_grant_ref unmap[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct page *pages[BLKIF_MAX_SEGMENTS_PER_REQUEST];
	struct persistent_gnt *persistent_gnt;
	int segs_to_unmap = 0;
	struct xen_blkif_ring *ring = container_of(work, typeof(*ring), persistent_purge_work);
	struct gntab_unmap_queue_data unmap_data;

	unmap_data.pages = pages;
	unmap_data.unmap_ops = unmap;
	unmap_data.kunmap_ops = NULL;

	while(!list_empty(&ring->persistent_purge_list)) {
		persistent_gnt = list_first_entry(&ring->persistent_purge_list,
		                                  struct persistent_gnt,
		                                  remove_node);
		list_del(&persistent_gnt->remove_node);

		gnttab_set_unmap_op(&unmap[segs_to_unmap],
			vaddr(persistent_gnt->page),
			GNTMAP_host_map,
			persistent_gnt->handle);

		pages[segs_to_unmap] = persistent_gnt->page;

		if (++segs_to_unmap == BLKIF_MAX_SEGMENTS_PER_REQUEST) {
			unmap_data.count = segs_to_unmap;
			BUG_ON(gnttab_unmap_refs_sync(&unmap_data));
			put_free_pages(ring, pages, segs_to_unmap);
			segs_to_unmap = 0;
		}
		kfree(persistent_gnt);
	}
	if (segs_to_unmap > 0) {
		unmap_data.count = segs_to_unmap;
		BUG_ON(gnttab_unmap_refs_sync(&unmap_data));
		put_free_pages(ring, pages, segs_to_unmap);
	}
}
