bsg_map_hdr(struct bsg_device *bd, struct sg_io_v4 *hdr, fmode_t has_write_perm,
	    u8 *sense)
{
	struct request_queue *q = bd->queue;
	struct request *rq, *next_rq = NULL;
	int ret, rw;
	unsigned int dxfer_len;
	void __user *dxferp = NULL;
	struct bsg_class_device *bcd = &q->bsg_dev;

	/* if the LLD has been removed then the bsg_unregister_queue will
	 * eventually be called and the class_dev was freed, so we can no
	 * longer use this request_queue. Return no such address.
	 */
	if (!bcd->class_dev)
		return ERR_PTR(-ENXIO);

	dprintk("map hdr %llx/%u %llx/%u\n", (unsigned long long) hdr->dout_xferp,
		hdr->dout_xfer_len, (unsigned long long) hdr->din_xferp,
		hdr->din_xfer_len);

	ret = bsg_validate_sgv4_hdr(hdr, &rw);
	if (ret)
		return ERR_PTR(ret);

	/*
	 * map scatter-gather elements separately and string them to request
	 */
	rq = blk_get_request(q, rw, GFP_KERNEL);
	if (IS_ERR(rq))
		return rq;
	blk_rq_set_block_pc(rq);

	ret = blk_fill_sgv4_hdr_rq(q, rq, hdr, bd, has_write_perm);
	if (ret)
		goto out;

	if (rw == WRITE && hdr->din_xfer_len) {
		if (!test_bit(QUEUE_FLAG_BIDI, &q->queue_flags)) {
			ret = -EOPNOTSUPP;
			goto out;
		}

		next_rq = blk_get_request(q, READ, GFP_KERNEL);
		if (IS_ERR(next_rq)) {
			ret = PTR_ERR(next_rq);
			next_rq = NULL;
			goto out;
		}
		rq->next_rq = next_rq;
		next_rq->cmd_type = rq->cmd_type;

		dxferp = (void __user *)(unsigned long)hdr->din_xferp;
		ret =  blk_rq_map_user(q, next_rq, NULL, dxferp,
				       hdr->din_xfer_len, GFP_KERNEL);
		if (ret)
			goto out;
	}

	if (hdr->dout_xfer_len) {
		dxfer_len = hdr->dout_xfer_len;
		dxferp = (void __user *)(unsigned long)hdr->dout_xferp;
	} else if (hdr->din_xfer_len) {
		dxfer_len = hdr->din_xfer_len;
		dxferp = (void __user *)(unsigned long)hdr->din_xferp;
	} else
		dxfer_len = 0;

	if (dxfer_len) {
		ret = blk_rq_map_user(q, rq, NULL, dxferp, dxfer_len,
				      GFP_KERNEL);
		if (ret)
			goto out;
	}

	rq->sense = sense;
	rq->sense_len = 0;

	return rq;
out:
	if (rq->cmd != rq->__cmd)
		kfree(rq->cmd);
	blk_put_request(rq);
	if (next_rq) {
		blk_rq_unmap_user(next_rq->bio);
		blk_put_request(next_rq);
	}
	return ERR_PTR(ret);
}
