int iov_iter_fault_in_readable(struct iov_iter *i, size_t bytes)
{
	size_t skip = i->iov_offset;
	const struct iovec *iov;
	int err;
	struct iovec v;

	if (!(i->type & (ITER_BVEC|ITER_KVEC))) {
		iterate_iovec(i, bytes, v, iov, skip, ({
			err = fault_in_pages_readable(v.iov_base, v.iov_len);
			if (unlikely(err))
			return err;
		0;}))
	}
	return 0;
}
