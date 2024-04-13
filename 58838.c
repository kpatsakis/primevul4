static void *__napi_alloc_frag(unsigned int fragsz, gfp_t gfp_mask)
{
	struct napi_alloc_cache *nc = this_cpu_ptr(&napi_alloc_cache);

	return page_frag_alloc(&nc->page, fragsz, gfp_mask);
}
