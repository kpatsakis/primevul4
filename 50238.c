int blk_rq_append_bio(struct request *rq, struct bio *bio)
{
	if (!rq->bio) {
		blk_rq_bio_prep(rq->q, rq, bio);
	} else {
		if (!ll_back_merge_fn(rq->q, rq, bio))
			return -EINVAL;

		rq->biotail->bi_next = bio;
		rq->biotail = bio;
		rq->__data_len += bio->bi_iter.bi_size;
	}

	return 0;
}
