unsigned long iov_iter_alignment(const struct iov_iter *i)
{
	const struct iovec *iov = i->iov;
	unsigned long res;
	size_t size = i->count;
	size_t n;

	if (!size)
		return 0;

	res = (unsigned long)iov->iov_base + i->iov_offset;
	n = iov->iov_len - i->iov_offset;
	if (n >= size)
		return res | size;
	size -= n;
	res |= n;
	while (size > (++iov)->iov_len) {
		res |= (unsigned long)iov->iov_base | iov->iov_len;
		size -= iov->iov_len;
	}
	res |= (unsigned long)iov->iov_base | size;
	return res;
}
