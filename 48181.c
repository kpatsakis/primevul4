static unsigned long ecryptfs_get_zeroed_pages(gfp_t gfp_mask,
					       unsigned int order)
{
	struct page *page;

	page = alloc_pages(gfp_mask | __GFP_ZERO, order);
	if (page)
		return (unsigned long) page_address(page);
	return 0;
}
