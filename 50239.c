int blk_rq_map_kern(struct request_queue *q, struct request *rq, void *kbuf,
		    unsigned int len, gfp_t gfp_mask)
{
	int reading = rq_data_dir(rq) == READ;
	unsigned long addr = (unsigned long) kbuf;
	int do_copy = 0;
	struct bio *bio;
	int ret;

	if (len > (queue_max_hw_sectors(q) << 9))
		return -EINVAL;
	if (!len || !kbuf)
		return -EINVAL;

	do_copy = !blk_rq_aligned(q, addr, len) || object_is_on_stack(kbuf);
	if (do_copy)
		bio = bio_copy_kern(q, kbuf, len, gfp_mask, reading);
	else
		bio = bio_map_kern(q, kbuf, len, gfp_mask);

	if (IS_ERR(bio))
		return PTR_ERR(bio);

	if (!reading)
		bio_set_op_attrs(bio, REQ_OP_WRITE, 0);

	if (do_copy)
		rq->cmd_flags |= REQ_COPY_USER;

	ret = blk_rq_append_bio(rq, bio);
	if (unlikely(ret)) {
		/* request is too big */
		bio_put(bio);
		return ret;
	}

	blk_queue_bounce(q, &rq->bio);
	return 0;
}
