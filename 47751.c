void iov_iter_init(struct iov_iter *i, int direction,
			const struct iovec *iov, unsigned long nr_segs,
			size_t count)
{
	/* It will get better.  Eventually... */
	if (segment_eq(get_fs(), KERNEL_DS))
		direction |= REQ_KERNEL;
	i->type = direction;
	i->iov = iov;
	i->nr_segs = nr_segs;
	i->iov_offset = 0;
	i->count = count;
}
