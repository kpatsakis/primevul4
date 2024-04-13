static int dispatch_other_io(struct xen_blkif_ring *ring,
			     struct blkif_request *req,
			     struct pending_req *pending_req)
{
	free_req(ring, pending_req);
	make_response(ring, req->u.other.id, req->operation,
		      BLKIF_RSP_EOPNOTSUPP);
	return -EIO;
}
