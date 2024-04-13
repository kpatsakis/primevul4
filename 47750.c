ssize_t iov_iter_get_pages_alloc(struct iov_iter *i,
		   struct page ***pages, size_t maxsize,
		   size_t *start)
{
	size_t offset = i->iov_offset;
	const struct iovec *iov = i->iov;
	size_t len;
	unsigned long addr;
	void *p;
	int n;
	int res;

	len = iov->iov_len - offset;
	if (len > i->count)
		len = i->count;
	if (len > maxsize)
		len = maxsize;
	addr = (unsigned long)iov->iov_base + offset;
	len += *start = addr & (PAGE_SIZE - 1);
	addr &= ~(PAGE_SIZE - 1);
	n = (len + PAGE_SIZE - 1) / PAGE_SIZE;
	
	p = kmalloc(n * sizeof(struct page *), GFP_KERNEL);
	if (!p)
		p = vmalloc(n * sizeof(struct page *));
	if (!p)
		return -ENOMEM;

	res = get_user_pages_fast(addr, n, (i->type & WRITE) != WRITE, p);
	if (unlikely(res < 0)) {
		kvfree(p);
		return res;
	}
	*pages = p;
	return (res == n ? len : res * PAGE_SIZE) - *start;
}
