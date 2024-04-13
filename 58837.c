void __kfree_skb_flush(void)
{
	struct napi_alloc_cache *nc = this_cpu_ptr(&napi_alloc_cache);

	/* flush skb_cache if containing objects */
	if (nc->skb_count) {
		kmem_cache_free_bulk(skbuff_head_cache, nc->skb_count,
				     nc->skb_cache);
		nc->skb_count = 0;
	}
}
