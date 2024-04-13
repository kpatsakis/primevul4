static int dispatch_other_io(struct xen_blkif *blkif,
			     struct blkif_request *req,
			     struct pending_req *pending_req)
{
	free_req(blkif, pending_req);
	make_response(blkif, req->u.other.id, req->operation,
		      BLKIF_RSP_EOPNOTSUPP);
	return -EIO;
}
