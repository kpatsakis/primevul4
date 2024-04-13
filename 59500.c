static ssize_t pipe_get_pages(struct iov_iter *i,
		   struct page **pages, size_t maxsize, unsigned maxpages,
		   size_t *start)
{
	unsigned npages;
	size_t capacity;
	int idx;

	if (!maxsize)
		return 0;

	if (!sanity(i))
		return -EFAULT;

	data_start(i, &idx, start);
	/* some of this one + all after this one */
	npages = ((i->pipe->curbuf - idx - 1) & (i->pipe->buffers - 1)) + 1;
	capacity = min(npages,maxpages) * PAGE_SIZE - *start;

	return __pipe_get_pages(i, min(maxsize, capacity), pages, idx, start);
}
