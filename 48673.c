static struct sk_buff *batadv_frag_create(struct sk_buff *skb,
					  struct batadv_frag_packet *frag_head,
					  unsigned int mtu)
{
	struct sk_buff *skb_fragment;
	unsigned header_size = sizeof(*frag_head);
	unsigned fragment_size = mtu - header_size;

	skb_fragment = netdev_alloc_skb(NULL, mtu + ETH_HLEN);
	if (!skb_fragment)
		goto err;

	skb->priority = TC_PRIO_CONTROL;

	/* Eat the last mtu-bytes of the skb */
	skb_reserve(skb_fragment, header_size + ETH_HLEN);
	skb_split(skb, skb_fragment, skb->len - fragment_size);

	/* Add the header */
	skb_push(skb_fragment, header_size);
	memcpy(skb_fragment->data, frag_head, header_size);

err:
	return skb_fragment;
}
