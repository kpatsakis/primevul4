static int __blk_rq_map_user_iov(struct request *rq,
		struct rq_map_data *map_data, struct iov_iter *iter,
		gfp_t gfp_mask, bool copy)
{
	struct request_queue *q = rq->q;
	struct bio *bio, *orig_bio;
	int ret;

	if (copy)
		bio = bio_copy_user_iov(q, map_data, iter, gfp_mask);
	else
		bio = bio_map_user_iov(q, iter, gfp_mask);

	if (IS_ERR(bio))
		return PTR_ERR(bio);

	if (map_data && map_data->null_mapped)
		bio_set_flag(bio, BIO_NULL_MAPPED);

	iov_iter_advance(iter, bio->bi_iter.bi_size);
	if (map_data)
		map_data->offset += bio->bi_iter.bi_size;

	orig_bio = bio;
	blk_queue_bounce(q, &bio);

	/*
	 * We link the bounce buffer in and could have to traverse it
	 * later so we have to get a ref to prevent it from being freed
	 */
	bio_get(bio);

	ret = blk_rq_append_bio(rq, bio);
	if (ret) {
		bio_endio(bio);
		__blk_rq_unmap_user(orig_bio);
		bio_put(bio);
		return ret;
	}

	return 0;
}
