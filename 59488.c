unsigned long iov_iter_alignment(const struct iov_iter *i)
{
	unsigned long res = 0;
	size_t size = i->count;

	if (unlikely(i->type & ITER_PIPE)) {
		if (size && i->iov_offset && allocated(&i->pipe->bufs[i->idx]))
			return size | i->iov_offset;
		return size;
	}
	iterate_all_kinds(i, size, v,
		(res |= (unsigned long)v.iov_base | v.iov_len, 0),
		res |= v.bv_offset | v.bv_len,
		res |= (unsigned long)v.iov_base | v.iov_len
	)
	return res;
}
