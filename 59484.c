static struct page **get_pages_array(size_t n)
{
	struct page **p = kmalloc(n * sizeof(struct page *), GFP_KERNEL);
	if (!p)
		p = vmalloc(n * sizeof(struct page *));
	return p;
}
