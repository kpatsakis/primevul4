int blk_rq_map_user(struct request_queue *q, struct request *rq,
		    struct rq_map_data *map_data, void __user *ubuf,
		    unsigned long len, gfp_t gfp_mask)
{
	struct iovec iov;
	struct iov_iter i;
	int ret = import_single_range(rq_data_dir(rq), ubuf, len, &iov, &i);

	if (unlikely(ret < 0))
		return ret;

	return blk_rq_map_user_iov(q, rq, map_data, &i, gfp_mask);
}
