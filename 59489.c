void iov_iter_bvec(struct iov_iter *i, int direction,
			const struct bio_vec *bvec, unsigned long nr_segs,
			size_t count)
{
	BUG_ON(!(direction & ITER_BVEC));
	i->type = direction;
	i->bvec = bvec;
	i->nr_segs = nr_segs;
	i->iov_offset = 0;
	i->count = count;
}
