static int blk_complete_sgv4_hdr_rq(struct request *rq, struct sg_io_v4 *hdr,
				    struct bio *bio, struct bio *bidi_bio)
{
	int ret = 0;

	dprintk("rq %p bio %p 0x%x\n", rq, bio, rq->errors);
	/*
	 * fill in all the output members
	 */
	hdr->device_status = rq->errors & 0xff;
	hdr->transport_status = host_byte(rq->errors);
	hdr->driver_status = driver_byte(rq->errors);
	hdr->info = 0;
	if (hdr->device_status || hdr->transport_status || hdr->driver_status)
		hdr->info |= SG_INFO_CHECK;
	hdr->response_len = 0;

	if (rq->sense_len && hdr->response) {
		int len = min_t(unsigned int, hdr->max_response_len,
					rq->sense_len);

		ret = copy_to_user((void __user *)(unsigned long)hdr->response,
				   rq->sense, len);
		if (!ret)
			hdr->response_len = len;
		else
			ret = -EFAULT;
	}

	if (rq->next_rq) {
		hdr->dout_resid = rq->resid_len;
		hdr->din_resid = rq->next_rq->resid_len;
		blk_rq_unmap_user(bidi_bio);
		blk_put_request(rq->next_rq);
	} else if (rq_data_dir(rq) == READ)
		hdr->din_resid = rq->resid_len;
	else
		hdr->dout_resid = rq->resid_len;

	/*
	 * If the request generated a negative error number, return it
	 * (providing we aren't already returning an error); if it's
	 * just a protocol response (i.e. non negative), that gets
	 * processed above.
	 */
	if (!ret && rq->errors < 0)
		ret = rq->errors;

	blk_rq_unmap_user(bio);
	if (rq->cmd != rq->__cmd)
		kfree(rq->cmd);
	blk_put_request(rq);

	return ret;
}
