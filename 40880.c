static int dispatch_rw_block_io(struct xen_blkif *blkif,
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
	struct blk_plug plug;
	bool drain = false;
	struct grant_page **pages = pending_req->segments;
	unsigned short req_operation;

	req_operation = req->operation == BLKIF_OP_INDIRECT ?
			req->u.indirect.indirect_op : req->operation;
	if ((req->operation == BLKIF_OP_INDIRECT) &&
	    (req_operation != BLKIF_OP_READ) &&
	    (req_operation != BLKIF_OP_WRITE)) {
		pr_debug(DRV_PFX "Invalid indirect operation (%u)\n",
			 req_operation);
		goto fail_response;
	}

	switch (req_operation) {
	case BLKIF_OP_READ:
		blkif->st_rd_req++;
		operation = READ;
		break;
	case BLKIF_OP_WRITE:
		blkif->st_wr_req++;
		operation = WRITE_ODIRECT;
		break;
	case BLKIF_OP_WRITE_BARRIER:
		drain = true;
	case BLKIF_OP_FLUSH_DISKCACHE:
		blkif->st_f_req++;
		operation = WRITE_FLUSH;
		break;
	default:
		operation = 0; /* make gcc happy */
		goto fail_response;
		break;
	}

	/* Check that the number of segments is sane. */
	nseg = req->operation == BLKIF_OP_INDIRECT ?
	       req->u.indirect.nr_segments : req->u.rw.nr_segments;

	if (unlikely(nseg == 0 && operation != WRITE_FLUSH) ||
	    unlikely((req->operation != BLKIF_OP_INDIRECT) &&
		     (nseg > BLKIF_MAX_SEGMENTS_PER_REQUEST)) ||
	    unlikely((req->operation == BLKIF_OP_INDIRECT) &&
		     (nseg > MAX_INDIRECT_SEGMENTS))) {
		pr_debug(DRV_PFX "Bad number of segments in request (%d)\n",
			 nseg);
		/* Haven't submitted any bio's yet. */
		goto fail_response;
	}

	preq.nr_sects      = 0;

	pending_req->blkif     = blkif;
	pending_req->id        = req->u.rw.id;
	pending_req->operation = req_operation;
	pending_req->status    = BLKIF_RSP_OKAY;
	pending_req->nr_pages  = nseg;

	if (req->operation != BLKIF_OP_INDIRECT) {
		preq.dev               = req->u.rw.handle;
		preq.sector_number     = req->u.rw.sector_number;
		for (i = 0; i < nseg; i++) {
			pages[i]->gref = req->u.rw.seg[i].gref;
			seg[i].nsec = req->u.rw.seg[i].last_sect -
				req->u.rw.seg[i].first_sect + 1;
			seg[i].offset = (req->u.rw.seg[i].first_sect << 9);
			if ((req->u.rw.seg[i].last_sect >= (PAGE_SIZE >> 9)) ||
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

	if (xen_vbd_translate(&preq, blkif, operation) != 0) {
		pr_debug(DRV_PFX "access denied: %s of [%llu,%llu] on dev=%04x\n",
			 operation == READ ? "read" : "write",
			 preq.sector_number,
			 preq.sector_number + preq.nr_sects,
			 blkif->vbd.pdevice);
		goto fail_response;
	}

	/*
	 * This check _MUST_ be done after xen_vbd_translate as the preq.bdev
	 * is set there.
	 */
	for (i = 0; i < nseg; i++) {
		if (((int)preq.sector_number|(int)seg[i].nsec) &
		    ((bdev_logical_block_size(preq.bdev) >> 9) - 1)) {
			pr_debug(DRV_PFX "Misaligned I/O request from domain %d",
				 blkif->domid);
			goto fail_response;
		}
	}

	/* Wait on all outstanding I/O's and once that has been completed
	 * issue the WRITE_FLUSH.
	 */
	if (drain)
		xen_blk_drain_io(pending_req->blkif);

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
	xen_blkif_get(blkif);

	for (i = 0; i < nseg; i++) {
		while ((bio == NULL) ||
		       (bio_add_page(bio,
				     pages[i]->page,
				     seg[i].nsec << 9,
				     seg[i].offset) == 0)) {

			bio = bio_alloc(GFP_KERNEL, nseg-i);
			if (unlikely(bio == NULL))
				goto fail_put_bio;

			biolist[nbio++] = bio;
			bio->bi_bdev    = preq.bdev;
			bio->bi_private = pending_req;
			bio->bi_end_io  = end_block_io_op;
			bio->bi_sector  = preq.sector_number;
		}

		preq.sector_number += seg[i].nsec;
	}

	/* This will be hit if the operation was a flush or discard. */
	if (!bio) {
		BUG_ON(operation != WRITE_FLUSH);

		bio = bio_alloc(GFP_KERNEL, 0);
		if (unlikely(bio == NULL))
			goto fail_put_bio;

		biolist[nbio++] = bio;
		bio->bi_bdev    = preq.bdev;
		bio->bi_private = pending_req;
		bio->bi_end_io  = end_block_io_op;
	}

	atomic_set(&pending_req->pendcnt, nbio);
	blk_start_plug(&plug);

	for (i = 0; i < nbio; i++)
		submit_bio(operation, biolist[i]);

	/* Let the I/Os go.. */
	blk_finish_plug(&plug);

	if (operation == READ)
		blkif->st_rd_sect += preq.nr_sects;
	else if (operation & WRITE)
		blkif->st_wr_sect += preq.nr_sects;

	return 0;

 fail_flush:
	xen_blkbk_unmap(blkif, pending_req->segments,
	                pending_req->nr_pages);
 fail_response:
	/* Haven't submitted any bio's yet. */
	make_response(blkif, req->u.rw.id, req_operation, BLKIF_RSP_ERROR);
	free_req(blkif, pending_req);
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
