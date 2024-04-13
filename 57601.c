static inline void *skcipher_map(struct scatter_walk *walk)
{
	struct page *page = scatterwalk_page(walk);

	return (PageHighMem(page) ? kmap_atomic(page) : page_address(page)) +
	       offset_in_page(walk->offset);
}
