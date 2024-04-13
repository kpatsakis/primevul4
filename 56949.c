static int dispatch_rw_block_io(struct xen_blkif_ring *ring,
				struct blkif_request *req,
				struct pending_req *pending_req)
{
	struct phys_req preq;
	struct seg_buf *seg = pending_req->seg;
	unsigned int nseg;
	struct bio *bio = NULL;
	struct bio **biolist = pending_req->biolist;
	int i, nbio = 0;
	int operation;
	int operation_flags = 0;
	struct blk_plug plug;
	bool drain = false;
	struct grant_page **pages = pending_req->segments;
	unsigned short req_operation;

	req_operation = req->operation == BLKIF_OP_INDIRECT ?
			req->u.indirect.indirect_op : req->operation;

	if ((req->operation == BLKIF_OP_INDIRECT) &&
	    (req_operation != BLKIF_OP_READ) &&
	    (req_operation != BLKIF_OP_WRITE)) {
		pr_debug("Invalid indirect operation (%u)\n", req_operation);
		goto fail_response;
	}

	switch (req_operation) {
	case BLKIF_OP_READ:
		ring->st_rd_req++;
		operation = REQ_OP_READ;
		break;
	case BLKIF_OP_WRITE:
		ring->st_wr_req++;
		operation = REQ_OP_WRITE;
		operation_flags = REQ_SYNC | REQ_IDLE;
		break;
	case BLKIF_OP_WRITE_BARRIER:
		drain = true;
	case BLKIF_OP_FLUSH_DISKCACHE:
		ring->st_f_req++;
		operation = REQ_OP_WRITE;
		operation_flags = REQ_PREFLUSH;
		break;
	default:
		operation = 0; /* make gcc happy */
		goto fail_response;
		break;
	}

	/* Check that the number of segments is sane. */
	nseg = req->operation == BLKIF_OP_INDIRECT ?
	       req->u.indirect.nr_segments : req->u.rw.nr_segments;

	if (unlikely(nseg == 0 && operation_flags != REQ_PREFLUSH) ||
	    unlikely((req->operation != BLKIF_OP_INDIRECT) &&
		     (nseg > BLKIF_MAX_SEGMENTS_PER_REQUEST)) ||
	    unlikely((req->operation == BLKIF_OP_INDIRECT) &&
		     (nseg > MAX_INDIRECT_SEGMENTS))) {
		pr_debug("Bad number of segments in request (%d)\n", nseg);
		/* Haven't submitted any bio's yet. */
		goto fail_response;
	}

	preq.nr_sects      = 0;

	pending_req->ring      = ring;
	pending_req->id        = req->u.rw.id;
	pending_req->operation = req_operation;
	pending_req->status    = BLKIF_RSP_OKAY;
	pending_req->nr_segs   = nseg;

	if (req->operation != BLKIF_OP_INDIRECT) {
		preq.dev               = req->u.rw.handle;
		preq.sector_number     = req->u.rw.sector_number;
		for (i = 0; i < nseg; i++) {
			pages[i]->gref = req->u.rw.seg[i].gref;
			seg[i].nsec = req->u.rw.seg[i].last_sect -
				req->u.rw.seg[i].first_sect + 1;
			seg[i].offset = (req->u.rw.seg[i].first_sect << 9);
			if ((req->u.rw.seg[i].last_sect >= (XEN_PAGE_SIZE >> 9)) ||
			    (req->u.rw.seg[i].last_sect <
			     req->u.rw.seg[i].first_sect))
				goto fail_response;
			preq.nr_sects += seg[i].nsec;
		}
	} else {
		preq.dev               = req->u.indirect.handle;
		preq.sector_number     = req->u.indirect.sector_number;
		if (xen_blkbk_parse_indirect(req, pending_req, seg, &preq))
			goto fail_response;
	}

	if (xen_vbd_translate(&preq, ring->blkif, operation) != 0) {
		pr_debug("access denied: %s of [%llu,%llu] on dev=%04x\n",
			 operation == REQ_OP_READ ? "read" : "write",
			 preq.sector_number,
			 preq.sector_number + preq.nr_sects,
			 ring->blkif->vbd.pdevice);
		goto fail_response;
	}

	/*
	 * This check _MUST_ be done after xen_vbd_translate as the preq.bdev
	 * is set there.
	 */
	for (i = 0; i < nseg; i++) {
		if (((int)preq.sector_number|(int)seg[i].nsec) &
		    ((bdev_logical_block_size(preq.bdev) >> 9) - 1)) {
			pr_debug("Misaligned I/O request from domain %d\n",
				 ring->blkif->domid);
			goto fail_response;
		}
	}

	/* Wait on all outstanding I/O's and once that has been completed
	 * issue the flush.
	 */
	if (drain)
		xen_blk_drain_io(pending_req->ring);

	/*
	 * If we have failed at this point, we need to undo the M2P override,
	 * set gnttab_set_unmap_op on all of the grant references and perform
	 * the hypercall to unmap the grants - that is all done in
	 * xen_blkbk_unmap.
	 */
	if (xen_blkbk_map_seg(pending_req))
		goto fail_flush;

	/*
	 * This corresponding xen_blkif_put is done in __end_block_io_op, or
	 * below (in "!bio") if we are handling a BLKIF_OP_DISCARD.
	 */
	xen_blkif_get(ring->blkif);
	atomic_inc(&ring->inflight);

	for (i = 0; i < nseg; i++) {
		while ((bio == NULL) ||
		       (bio_add_page(bio,
				     pages[i]->page,
				     seg[i].nsec << 9,
				     seg[i].offset) == 0)) {

			int nr_iovecs = min_t(int, (nseg-i), BIO_MAX_PAGES);
			bio = bio_alloc(GFP_KERNEL, nr_iovecs);
			if (unlikely(bio == NULL))
				goto fail_put_bio;

			biolist[nbio++] = bio;
			bio->bi_bdev    = preq.bdev;
			bio->bi_private = pending_req;
			bio->bi_end_io  = end_block_io_op;
			bio->bi_iter.bi_sector  = preq.sector_number;
			bio_set_op_attrs(bio, operation, operation_flags);
		}

		preq.sector_number += seg[i].nsec;
	}

	/* This will be hit if the operation was a flush or discard. */
	if (!bio) {
		BUG_ON(operation_flags != REQ_PREFLUSH);

		bio = bio_alloc(GFP_KERNEL, 0);
		if (unlikely(bio == NULL))
			goto fail_put_bio;

		biolist[nbio++] = bio;
		bio->bi_bdev    = preq.bdev;
		bio->bi_private = pending_req;
		bio->bi_end_io  = end_block_io_op;
		bio_set_op_attrs(bio, operation, operation_flags);
	}

	atomic_set(&pending_req->pendcnt, nbio);
	blk_start_plug(&plug);

	for (i = 0; i < nbio; i++)
		submit_bio(biolist[i]);

	/* Let the I/Os go.. */
	blk_finish_plug(&plug);

	if (operation == REQ_OP_READ)
		ring->st_rd_sect += preq.nr_sects;
	else if (operation == REQ_OP_WRITE)
		ring->st_wr_sect += preq.nr_sects;

	return 0;

 fail_flush:
	xen_blkbk_unmap(ring, pending_req->segments,
	                pending_req->nr_segs);
 fail_response:
	/* Haven't submitted any bio's yet. */
	make_response(ring, req->u.rw.id, req_operation, BLKIF_RSP_ERROR);
	free_req(ring, pending_req);
	msleep(1); /* back off a bit */
	return -EIO;

 fail_put_bio:
	for (i = 0; i < nbio; i++)
		bio_put(biolist[i]);
	atomic_set(&pending_req->pendcnt, 1);
	__end_block_io_op(pending_req, -EINVAL);
	msleep(1); /* back off a bit */
	return -EIO;
}
