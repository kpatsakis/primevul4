static void *alloc_one_pg_vec_page(unsigned long order)
{
	void *buffer;
	gfp_t gfp_flags = GFP_KERNEL | __GFP_COMP | __GFP_ZERO |
			  __GFP_NOWARN | __GFP_NORETRY;

	buffer = (void *)__get_free_pages(gfp_flags, order);
	if (buffer != NULL)
		return buffer;

	buffer = vzalloc((1 << order) * PAGE_SIZE);
	if (buffer != NULL)
		return buffer;

	gfp_flags &= ~__GFP_NORETRY;
	return (void *)__get_free_pages(gfp_flags, order);
}
