static int bnep_rx_frame(struct bnep_session *s, struct sk_buff *skb)
{
	struct net_device *dev = s->dev;
	struct sk_buff *nskb;
	u8 type;

	dev->stats.rx_bytes += skb->len;

	type = *(u8 *) skb->data;
	skb_pull(skb, 1);

	if ((type & BNEP_TYPE_MASK) >= sizeof(__bnep_rx_hlen))
		goto badframe;

	if ((type & BNEP_TYPE_MASK) == BNEP_CONTROL) {
		bnep_rx_control(s, skb->data, skb->len);
		kfree_skb(skb);
		return 0;
	}

	skb_reset_mac_header(skb);

	/* Verify and pull out header */
	if (!skb_pull(skb, __bnep_rx_hlen[type & BNEP_TYPE_MASK]))
		goto badframe;

	s->eh.h_proto = get_unaligned((__be16 *) (skb->data - 2));

	if (type & BNEP_EXT_HEADER) {
		if (bnep_rx_extension(s, skb) < 0)
			goto badframe;
	}

	/* Strip 802.1p header */
	if (ntohs(s->eh.h_proto) == ETH_P_8021Q) {
		if (!skb_pull(skb, 4))
			goto badframe;
		s->eh.h_proto = get_unaligned((__be16 *) (skb->data - 2));
	}

	/* We have to alloc new skb and copy data here :(. Because original skb
	 * may not be modified and because of the alignment requirements. */
	nskb = alloc_skb(2 + ETH_HLEN + skb->len, GFP_KERNEL);
	if (!nskb) {
		dev->stats.rx_dropped++;
		kfree_skb(skb);
		return -ENOMEM;
	}
	skb_reserve(nskb, 2);

	/* Decompress header and construct ether frame */
	switch (type & BNEP_TYPE_MASK) {
	case BNEP_COMPRESSED:
		memcpy(__skb_put(nskb, ETH_HLEN), &s->eh, ETH_HLEN);
		break;

	case BNEP_COMPRESSED_SRC_ONLY:
		memcpy(__skb_put(nskb, ETH_ALEN), s->eh.h_dest, ETH_ALEN);
		memcpy(__skb_put(nskb, ETH_ALEN), skb_mac_header(skb), ETH_ALEN);
		put_unaligned(s->eh.h_proto, (__be16 *) __skb_put(nskb, 2));
		break;

	case BNEP_COMPRESSED_DST_ONLY:
		memcpy(__skb_put(nskb, ETH_ALEN), skb_mac_header(skb),
								ETH_ALEN);
		memcpy(__skb_put(nskb, ETH_ALEN + 2), s->eh.h_source,
								ETH_ALEN + 2);
		break;

	case BNEP_GENERAL:
		memcpy(__skb_put(nskb, ETH_ALEN * 2), skb_mac_header(skb),
								ETH_ALEN * 2);
		put_unaligned(s->eh.h_proto, (__be16 *) __skb_put(nskb, 2));
		break;
	}

	skb_copy_from_linear_data(skb, __skb_put(nskb, skb->len), skb->len);
	kfree_skb(skb);

	dev->stats.rx_packets++;
	nskb->ip_summed = CHECKSUM_NONE;
	nskb->protocol  = eth_type_trans(nskb, dev);
	netif_rx_ni(nskb);
	return 0;

badframe:
	dev->stats.rx_errors++;
	kfree_skb(skb);
	return 0;
}
