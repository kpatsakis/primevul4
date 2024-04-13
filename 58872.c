static void skb_release_data(struct sk_buff *skb)
{
	struct skb_shared_info *shinfo = skb_shinfo(skb);
	int i;

	if (skb->cloned &&
	    atomic_sub_return(skb->nohdr ? (1 << SKB_DATAREF_SHIFT) + 1 : 1,
			      &shinfo->dataref))
		return;

	for (i = 0; i < shinfo->nr_frags; i++)
		__skb_frag_unref(&shinfo->frags[i]);

	/*
	 * If skb buf is from userspace, we need to notify the caller
	 * the lower device DMA has done;
	 */
	if (shinfo->tx_flags & SKBTX_DEV_ZEROCOPY) {
		struct ubuf_info *uarg;

		uarg = shinfo->destructor_arg;
		if (uarg->callback)
			uarg->callback(uarg, true);
	}

	if (shinfo->frag_list)
		kfree_skb_list(shinfo->frag_list);

	skb_free_head(skb);
}
