static int page_not_mapped(struct page *page)
{
	return !page_mapped(page);
};
