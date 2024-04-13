__do_block_io_op(struct xen_blkif_ring *ring)
{
	union blkif_back_rings *blk_rings = &ring->blk_rings;
	struct blkif_request req;
	struct pending_req *pending_req;
	RING_IDX rc, rp;
	int more_to_do = 0;

	rc = blk_rings->common.req_cons;
	rp = blk_rings->common.sring->req_prod;
	rmb(); /* Ensure we see queued requests up to 'rp'. */

	if (RING_REQUEST_PROD_OVERFLOW(&blk_rings->common, rp)) {
		rc = blk_rings->common.rsp_prod_pvt;
		pr_warn("Frontend provided bogus ring requests (%d - %d = %d). Halting ring processing on dev=%04x\n",
			rp, rc, rp - rc, ring->blkif->vbd.pdevice);
		return -EACCES;
	}
	while (rc != rp) {

		if (RING_REQUEST_CONS_OVERFLOW(&blk_rings->common, rc))
			break;

		if (kthread_should_stop()) {
			more_to_do = 1;
			break;
		}

		pending_req = alloc_req(ring);
		if (NULL == pending_req) {
			ring->st_oo_req++;
			more_to_do = 1;
			break;
		}

		switch (ring->blkif->blk_protocol) {
		case BLKIF_PROTOCOL_NATIVE:
			memcpy(&req, RING_GET_REQUEST(&blk_rings->native, rc), sizeof(req));
			break;
		case BLKIF_PROTOCOL_X86_32:
			blkif_get_x86_32_req(&req, RING_GET_REQUEST(&blk_rings->x86_32, rc));
			break;
		case BLKIF_PROTOCOL_X86_64:
			blkif_get_x86_64_req(&req, RING_GET_REQUEST(&blk_rings->x86_64, rc));
			break;
		default:
			BUG();
		}
		blk_rings->common.req_cons = ++rc; /* before make_response() */

		/* Apply all sanity checks to /private copy/ of request. */
		barrier();

		switch (req.operation) {
		case BLKIF_OP_READ:
		case BLKIF_OP_WRITE:
		case BLKIF_OP_WRITE_BARRIER:
		case BLKIF_OP_FLUSH_DISKCACHE:
		case BLKIF_OP_INDIRECT:
			if (dispatch_rw_block_io(ring, &req, pending_req))
				goto done;
			break;
		case BLKIF_OP_DISCARD:
			free_req(ring, pending_req);
			if (dispatch_discard_io(ring, &req))
				goto done;
			break;
		default:
			if (dispatch_other_io(ring, &req, pending_req))
				goto done;
			break;
		}

		/* Yield point for this unbounded loop. */
		cond_resched();
	}
done:
	return more_to_do;
}
