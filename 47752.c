int iov_iter_npages(const struct iov_iter *i, int maxpages)
{
	size_t offset = i->iov_offset;
	size_t size = i->count;
	const struct iovec *iov = i->iov;
	int npages = 0;
	int n;

	for (n = 0; size && n < i->nr_segs; n++, iov++) {
		unsigned long addr = (unsigned long)iov->iov_base + offset;
		size_t len = iov->iov_len - offset;
		offset = 0;
		if (unlikely(!len))	/* empty segment */
			continue;
		if (len > size)
			len = size;
		npages += (addr + len + PAGE_SIZE - 1) / PAGE_SIZE
			  - addr / PAGE_SIZE;
		if (npages >= maxpages)	/* don't bother going further */
			return maxpages;
		size -= len;
		offset = 0;
	}
	return min(npages, maxpages);
}
