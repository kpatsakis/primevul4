static int xen_blkbk_map_seg(struct pending_req *pending_req)
{
	int rc;

	rc = xen_blkbk_map(pending_req->ring, pending_req->segments,
			   pending_req->nr_segs,
	                   (pending_req->operation != BLKIF_OP_READ));

	return rc;
}
