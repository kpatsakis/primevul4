ssize_t iov_iter_get_pages(struct iov_iter *i,
		   struct page **pages, size_t maxsize, unsigned maxpages,
		   size_t *start)
{
	if (maxsize > i->count)
		maxsize = i->count;

	if (unlikely(i->type & ITER_PIPE))
		return pipe_get_pages(i, pages, maxsize, maxpages, start);
	iterate_all_kinds(i, maxsize, v, ({
		unsigned long addr = (unsigned long)v.iov_base;
		size_t len = v.iov_len + (*start = addr & (PAGE_SIZE - 1));
		int n;
		int res;

		if (len > maxpages * PAGE_SIZE)
			len = maxpages * PAGE_SIZE;
		addr &= ~(PAGE_SIZE - 1);
		n = DIV_ROUND_UP(len, PAGE_SIZE);
		res = get_user_pages_fast(addr, n, (i->type & WRITE) != WRITE, pages);
		if (unlikely(res < 0))
			return res;
		return (res == n ? len : res * PAGE_SIZE) - *start;
	0;}),({
		/* can't be more than PAGE_SIZE */
		*start = v.bv_offset;
		get_page(*pages = v.bv_page);
		return v.bv_len;
	}),({
		return -EFAULT;
	})
	)
	return 0;
}
