struct sk_buff *__netdev_alloc_skb(struct net_device *dev, unsigned int len,
				   gfp_t gfp_mask)
{
	struct page_frag_cache *nc;
	unsigned long flags;
	struct sk_buff *skb;
	bool pfmemalloc;
	void *data;

	len += NET_SKB_PAD;

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

	local_irq_save(flags);

	nc = this_cpu_ptr(&netdev_alloc_cache);
	data = page_frag_alloc(nc, len, gfp_mask);
	pfmemalloc = nc->pfmemalloc;

	local_irq_restore(flags);

	if (unlikely(!data))
		return NULL;

	skb = __build_skb(data, len);
	if (unlikely(!skb)) {
		skb_free_frag(data);
		return NULL;
	}

	/* use OR instead of assignment to avoid clearing of bits in mask */
	if (pfmemalloc)
		skb->pfmemalloc = 1;
	skb->head_frag = 1;

skb_success:
	skb_reserve(skb, NET_SKB_PAD);
	skb->dev = dev;

skb_fail:
	return skb;
}
