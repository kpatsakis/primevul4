int iov_iter_npages(const struct iov_iter *i, int maxpages)
{
	size_t size = i->count;
	int npages = 0;

	if (!size)
		return 0;

	if (unlikely(i->type & ITER_PIPE)) {
		struct pipe_inode_info *pipe = i->pipe;
		size_t off;
		int idx;

		if (!sanity(i))
			return 0;

		data_start(i, &idx, &off);
		/* some of this one + all after this one */
		npages = ((pipe->curbuf - idx - 1) & (pipe->buffers - 1)) + 1;
		if (npages >= maxpages)
			return maxpages;
	} else iterate_all_kinds(i, size, v, ({
		unsigned long p = (unsigned long)v.iov_base;
		npages += DIV_ROUND_UP(p + v.iov_len, PAGE_SIZE)
			- p / PAGE_SIZE;
		if (npages >= maxpages)
			return maxpages;
	0;}),({
		npages++;
		if (npages >= maxpages)
			return maxpages;
	}),({
		unsigned long p = (unsigned long)v.iov_base;
		npages += DIV_ROUND_UP(p + v.iov_len, PAGE_SIZE)
			- p / PAGE_SIZE;
		if (npages >= maxpages)
			return maxpages;
	})
	)
	return npages;
}
