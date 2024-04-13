struct sk_buff *alloc_skb_with_frags(unsigned long header_len,
				     unsigned long data_len,
				     int max_page_order,
				     int *errcode,
				     gfp_t gfp_mask)
{
	int npages = (data_len + (PAGE_SIZE - 1)) >> PAGE_SHIFT;
	unsigned long chunk;
	struct sk_buff *skb;
	struct page *page;
	gfp_t gfp_head;
	int i;

	*errcode = -EMSGSIZE;
	/* Note this test could be relaxed, if we succeed to allocate
	 * high order pages...
	 */
	if (npages > MAX_SKB_FRAGS)
		return NULL;

	gfp_head = gfp_mask;
	if (gfp_head & __GFP_DIRECT_RECLAIM)
		gfp_head |= __GFP_REPEAT;

	*errcode = -ENOBUFS;
	skb = alloc_skb(header_len, gfp_head);
	if (!skb)
		return NULL;

	skb->truesize += npages << PAGE_SHIFT;

	for (i = 0; npages > 0; i++) {
		int order = max_page_order;

		while (order) {
			if (npages >= 1 << order) {
				page = alloc_pages((gfp_mask & ~__GFP_DIRECT_RECLAIM) |
						   __GFP_COMP |
						   __GFP_NOWARN |
						   __GFP_NORETRY,
						   order);
				if (page)
					goto fill_page;
				/* Do not retry other high order allocations */
				order = 1;
				max_page_order = 0;
			}
			order--;
		}
		page = alloc_page(gfp_mask);
		if (!page)
			goto failure;
fill_page:
		chunk = min_t(unsigned long, data_len,
			      PAGE_SIZE << order);
		skb_fill_page_desc(skb, i, page, 0, chunk);
		data_len -= chunk;
		npages -= 1 << order;
	}
	return skb;

failure:
	kfree_skb(skb);
	return NULL;
}
