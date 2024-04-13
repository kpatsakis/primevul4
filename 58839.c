struct sk_buff *__napi_alloc_skb(struct napi_struct *napi, unsigned int len,
				 gfp_t gfp_mask)
{
	struct napi_alloc_cache *nc = this_cpu_ptr(&napi_alloc_cache);
	struct sk_buff *skb;
	void *data;

	len += NET_SKB_PAD + NET_IP_ALIGN;

	if ((len > SKB_WITH_OVERHEAD(PAGE_SIZE)) ||
	    (gfp_mask & (__GFP_DIRECT_RECLAIM | GFP_DMA))) {
		skb = __alloc_skb(len, gfp_mask, SKB_ALLOC_RX, NUMA_NO_NODE);
		if (!skb)
			goto skb_fail;
		goto skb_success;
	}

	len += SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
	len = SKB_DATA_ALIGN(len);

	if (sk_memalloc_socks())
		gfp_mask |= __GFP_MEMALLOC;

	data = page_frag_alloc(&nc->page, len, gfp_mask);
	if (unlikely(!data))
		return NULL;

	skb = __build_skb(data, len);
	if (unlikely(!skb)) {
		skb_free_frag(data);
		return NULL;
	}

	/* use OR instead of assignment to avoid clearing of bits in mask */
	if (nc->page.pfmemalloc)
		skb->pfmemalloc = 1;
	skb->head_frag = 1;

skb_success:
	skb_reserve(skb, NET_SKB_PAD + NET_IP_ALIGN);
	skb->dev = napi->dev;

skb_fail:
	return skb;
}
