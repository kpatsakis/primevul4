static int md_mergeable_bvec(struct request_queue *q,
			     struct bvec_merge_data *bvm,
			     struct bio_vec *biovec)
{
	struct mddev *mddev = q->queuedata;
	int ret;
	rcu_read_lock();
	if (mddev->suspended) {
		/* Must always allow one vec */
		if (bvm->bi_size == 0)
			ret = biovec->bv_len;
		else
			ret = 0;
	} else {
		struct md_personality *pers = mddev->pers;
		if (pers && pers->mergeable_bvec)
			ret = pers->mergeable_bvec(mddev, bvm, biovec);
		else
			ret = biovec->bv_len;
	}
	rcu_read_unlock();
	return ret;
}
