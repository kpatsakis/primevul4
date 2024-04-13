static int xen_blkbk_map_seg(struct pending_req *pending_req)
{
	int rc;

	rc = xen_blkbk_map(pending_req->blkif, pending_req->segments,
			   pending_req->nr_pages,
	                   (pending_req->operation != BLKIF_OP_READ));

	return rc;
}
