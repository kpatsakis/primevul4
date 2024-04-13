void iov_iter_kvec(struct iov_iter *i, int direction,
			const struct kvec *kvec, unsigned long nr_segs,
			size_t count)
{
	BUG_ON(!(direction & ITER_KVEC));
	i->type = direction;
	i->kvec = kvec;
	i->nr_segs = nr_segs;
	i->iov_offset = 0;
	i->count = count;
}
