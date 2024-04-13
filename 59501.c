static ssize_t pipe_get_pages_alloc(struct iov_iter *i,
		   struct page ***pages, size_t maxsize,
		   size_t *start)
{
	struct page **p;
	size_t n;
	int idx;
	int npages;

	if (!maxsize)
		return 0;

	if (!sanity(i))
		return -EFAULT;

	data_start(i, &idx, start);
	/* some of this one + all after this one */
	npages = ((i->pipe->curbuf - idx - 1) & (i->pipe->buffers - 1)) + 1;
	n = npages * PAGE_SIZE - *start;
	if (maxsize > n)
		maxsize = n;
	else
		npages = DIV_ROUND_UP(maxsize + *start, PAGE_SIZE);
	p = get_pages_array(npages);
	if (!p)
		return -ENOMEM;
	n = __pipe_get_pages(i, maxsize, p, idx, start);
	if (n > 0)
		*pages = p;
	else
		kvfree(p);
	return n;
}
