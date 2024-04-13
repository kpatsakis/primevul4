ssize_t iov_iter_get_pages_alloc(struct iov_iter *i,
		   struct page ***pages, size_t maxsize,
		   size_t *start)
{
	struct page **p;

	if (maxsize > i->count)
		maxsize = i->count;

	if (unlikely(i->type & ITER_PIPE))
		return pipe_get_pages_alloc(i, pages, maxsize, start);
	iterate_all_kinds(i, maxsize, v, ({
		unsigned long addr = (unsigned long)v.iov_base;
		size_t len = v.iov_len + (*start = addr & (PAGE_SIZE - 1));
		int n;
		int res;

		addr &= ~(PAGE_SIZE - 1);
		n = DIV_ROUND_UP(len, PAGE_SIZE);
		p = get_pages_array(n);
		if (!p)
			return -ENOMEM;
		res = get_user_pages_fast(addr, n, (i->type & WRITE) != WRITE, p);
		if (unlikely(res < 0)) {
			kvfree(p);
			return res;
		}
		*pages = p;
		return (res == n ? len : res * PAGE_SIZE) - *start;
	0;}),({
		/* can't be more than PAGE_SIZE */
		*start = v.bv_offset;
		*pages = p = get_pages_array(1);
		if (!p)
			return -ENOMEM;
		get_page(*p = v.bv_page);
		return v.bv_len;
	}),({
		return -EFAULT;
	})
	)
	return 0;
}
