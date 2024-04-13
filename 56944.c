static void __end_block_io_op(struct pending_req *pending_req, int error)
{
	/* An error fails the entire request. */
	if ((pending_req->operation == BLKIF_OP_FLUSH_DISKCACHE) &&
	    (error == -EOPNOTSUPP)) {
		pr_debug("flush diskcache op failed, not supported\n");
		xen_blkbk_flush_diskcache(XBT_NIL, pending_req->ring->blkif->be, 0);
		pending_req->status = BLKIF_RSP_EOPNOTSUPP;
	} else if ((pending_req->operation == BLKIF_OP_WRITE_BARRIER) &&
		    (error == -EOPNOTSUPP)) {
		pr_debug("write barrier op failed, not supported\n");
		xen_blkbk_barrier(XBT_NIL, pending_req->ring->blkif->be, 0);
		pending_req->status = BLKIF_RSP_EOPNOTSUPP;
	} else if (error) {
		pr_debug("Buffer not up-to-date at end of operation,"
			 " error=%d\n", error);
		pending_req->status = BLKIF_RSP_ERROR;
	}

	/*
	 * If all of the bio's have completed it is time to unmap
	 * the grant references associated with 'request' and provide
	 * the proper response on the ring.
	 */
	if (atomic_dec_and_test(&pending_req->pendcnt))
		xen_blkbk_unmap_and_respond(pending_req);
}
