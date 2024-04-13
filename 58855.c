int skb_append_pagefrags(struct sk_buff *skb, struct page *page,
			 int offset, size_t size)
{
	int i = skb_shinfo(skb)->nr_frags;

	if (skb_can_coalesce(skb, i, page, offset)) {
		skb_frag_size_add(&skb_shinfo(skb)->frags[i - 1], size);
	} else if (i < MAX_SKB_FRAGS) {
		get_page(page);
		skb_fill_page_desc(skb, i, page, offset, size);
	} else {
		return -EMSGSIZE;
	}

	return 0;
}
