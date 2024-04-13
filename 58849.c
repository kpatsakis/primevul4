void *napi_alloc_frag(unsigned int fragsz)
{
	return __napi_alloc_frag(fragsz, GFP_ATOMIC | __GFP_COLD);
}
