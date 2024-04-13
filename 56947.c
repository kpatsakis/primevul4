static int dispatch_discard_io(struct xen_blkif_ring *ring,
				struct blkif_request *req)
{
	int err = 0;
	int status = BLKIF_RSP_OKAY;
	struct xen_blkif *blkif = ring->blkif;
	struct block_device *bdev = blkif->vbd.bdev;
	unsigned long secure;
	struct phys_req preq;

	xen_blkif_get(blkif);

	preq.sector_number = req->u.discard.sector_number;
	preq.nr_sects      = req->u.discard.nr_sectors;

	err = xen_vbd_translate(&preq, blkif, REQ_OP_WRITE);
	if (err) {
		pr_warn("access denied: DISCARD [%llu->%llu] on dev=%04x\n",
			preq.sector_number,
			preq.sector_number + preq.nr_sects, blkif->vbd.pdevice);
		goto fail_response;
	}
	ring->st_ds_req++;

	secure = (blkif->vbd.discard_secure &&
		 (req->u.discard.flag & BLKIF_DISCARD_SECURE)) ?
		 BLKDEV_DISCARD_SECURE : 0;

	err = blkdev_issue_discard(bdev, req->u.discard.sector_number,
				   req->u.discard.nr_sectors,
				   GFP_KERNEL, secure);
fail_response:
	if (err == -EOPNOTSUPP) {
		pr_debug("discard op failed, not supported\n");
		status = BLKIF_RSP_EOPNOTSUPP;
	} else if (err)
		status = BLKIF_RSP_ERROR;

	make_response(ring, req->u.discard.id, req->operation, status);
	xen_blkif_put(blkif);
	return err;
}
