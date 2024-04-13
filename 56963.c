static void xen_blkbk_unmap_and_respond(struct pending_req *req)
{
	struct gntab_unmap_queue_data* work = &req->gnttab_unmap_data;
	struct xen_blkif_ring *ring = req->ring;
	struct grant_page **pages = req->segments;
	unsigned int invcount;

	invcount = xen_blkbk_unmap_prepare(ring, pages, req->nr_segs,
					   req->unmap, req->unmap_pages);

	work->data = req;
	work->done = xen_blkbk_unmap_and_respond_callback;
	work->unmap_ops = req->unmap;
	work->kunmap_ops = NULL;
	work->pages = req->unmap_pages;
	work->count = invcount;

	gnttab_unmap_refs_async(&req->gnttab_unmap_data);
}
