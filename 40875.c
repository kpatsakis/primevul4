static void __end_block_io_op(struct pending_req *pending_req, int error)
{
	/* An error fails the entire request. */
	if ((pending_req->operation == BLKIF_OP_FLUSH_DISKCACHE) &&
	    (error == -EOPNOTSUPP)) {
		pr_debug(DRV_PFX "flush diskcache op failed, not supported\n");
		xen_blkbk_flush_diskcache(XBT_NIL, pending_req->blkif->be, 0);
		pending_req->status = BLKIF_RSP_EOPNOTSUPP;
	} else if ((pending_req->operation == BLKIF_OP_WRITE_BARRIER) &&
		    (error == -EOPNOTSUPP)) {
		pr_debug(DRV_PFX "write barrier op failed, not supported\n");
		xen_blkbk_barrier(XBT_NIL, pending_req->blkif->be, 0);
		pending_req->status = BLKIF_RSP_EOPNOTSUPP;
	} else if (error) {
		pr_debug(DRV_PFX "Buffer not up-to-date at end of operation,"
			 " error=%d\n", error);
		pending_req->status = BLKIF_RSP_ERROR;
	}

	/*
	 * If all of the bio's have completed it is time to unmap
	 * the grant references associated with 'request' and provide
	 * the proper response on the ring.
	 */
	if (atomic_dec_and_test(&pending_req->pendcnt)) {
		xen_blkbk_unmap(pending_req->blkif,
		                pending_req->segments,
		                pending_req->nr_pages);
		make_response(pending_req->blkif, pending_req->id,
			      pending_req->operation, pending_req->status);
		xen_blkif_put(pending_req->blkif);
		if (atomic_read(&pending_req->blkif->refcnt) <= 2) {
			if (atomic_read(&pending_req->blkif->drain))
				complete(&pending_req->blkif->drain_complete);
		}
		free_req(pending_req->blkif, pending_req);
	}
}
