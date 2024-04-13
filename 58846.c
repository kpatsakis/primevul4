struct sk_buff *build_skb(void *data, unsigned int frag_size)
{
	struct sk_buff *skb = __build_skb(data, frag_size);

	if (skb && frag_size) {
		skb->head_frag = 1;
		if (page_is_pfmemalloc(virt_to_head_page(data)))
			skb->pfmemalloc = 1;
	}
	return skb;
}
