static inline void _kfree_skb_defer(struct sk_buff *skb)
{
	struct napi_alloc_cache *nc = this_cpu_ptr(&napi_alloc_cache);

	/* drop skb->head and call any destructors for packet */
	skb_release_all(skb);

	/* record skb to CPU local list */
	nc->skb_cache[nc->skb_count++] = skb;

#ifdef CONFIG_SLUB
	/* SLUB writes into objects when freeing */
	prefetchw(skb);
#endif

	/* flush skb_cache if it is filled */
	if (unlikely(nc->skb_count == NAPI_SKB_CACHE_SIZE)) {
		kmem_cache_free_bulk(skbuff_head_cache, NAPI_SKB_CACHE_SIZE,
				     nc->skb_cache);
		nc->skb_count = 0;
	}
}
