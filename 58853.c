static int pskb_carve_inside_header(struct sk_buff *skb, const u32 off,
				    const int headlen, gfp_t gfp_mask)
{
	int i;
	int size = skb_end_offset(skb);
	int new_hlen = headlen - off;
	u8 *data;

	size = SKB_DATA_ALIGN(size);

	if (skb_pfmemalloc(skb))
		gfp_mask |= __GFP_MEMALLOC;
	data = kmalloc_reserve(size +
			       SKB_DATA_ALIGN(sizeof(struct skb_shared_info)),
			       gfp_mask, NUMA_NO_NODE, NULL);
	if (!data)
		return -ENOMEM;

	size = SKB_WITH_OVERHEAD(ksize(data));

	/* Copy real data, and all frags */
	skb_copy_from_linear_data_offset(skb, off, data, new_hlen);
	skb->len -= off;

	memcpy((struct skb_shared_info *)(data + size),
	       skb_shinfo(skb),
	       offsetof(struct skb_shared_info,
			frags[skb_shinfo(skb)->nr_frags]));
	if (skb_cloned(skb)) {
		/* drop the old head gracefully */
		if (skb_orphan_frags(skb, gfp_mask)) {
			kfree(data);
			return -ENOMEM;
		}
		for (i = 0; i < skb_shinfo(skb)->nr_frags; i++)
			skb_frag_ref(skb, i);
		if (skb_has_frag_list(skb))
			skb_clone_fraglist(skb);
		skb_release_data(skb);
	} else {
		/* we can reuse existing recount- all we did was
		 * relocate values
		 */
		skb_free_head(skb);
	}

	skb->head = data;
	skb->data = data;
	skb->head_frag = 0;
#ifdef NET_SKBUFF_DATA_USES_OFFSET
	skb->end = size;
#else
	skb->end = skb->head + size;
#endif
	skb_set_tail_pointer(skb, skb_headlen(skb));
	skb_headers_offset_update(skb, 0);
	skb->cloned = 0;
	skb->hdr_len = 0;
	skb->nohdr = 0;
	atomic_set(&skb_shinfo(skb)->dataref, 1);

	return 0;
}
