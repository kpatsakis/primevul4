static int bnep_tx_frame(struct bnep_session *s, struct sk_buff *skb)
{
	struct ethhdr *eh = (void *) skb->data;
	struct socket *sock = s->sock;
	struct kvec iv[3];
	int len = 0, il = 0;
	u8 type = 0;

	BT_DBG("skb %p dev %p type %d", skb, skb->dev, skb->pkt_type);

	if (!skb->dev) {
		/* Control frame sent by us */
		goto send;
	}

	iv[il++] = (struct kvec) { &type, 1 };
	len++;

	if (compress_src && ether_addr_equal(eh->h_dest, s->eh.h_source))
		type |= 0x01;

	if (compress_dst && ether_addr_equal(eh->h_source, s->eh.h_dest))
		type |= 0x02;

	if (type)
		skb_pull(skb, ETH_ALEN * 2);

	type = __bnep_tx_types[type];
	switch (type) {
	case BNEP_COMPRESSED_SRC_ONLY:
		iv[il++] = (struct kvec) { eh->h_source, ETH_ALEN };
		len += ETH_ALEN;
		break;

	case BNEP_COMPRESSED_DST_ONLY:
		iv[il++] = (struct kvec) { eh->h_dest, ETH_ALEN };
		len += ETH_ALEN;
		break;
	}

send:
	iv[il++] = (struct kvec) { skb->data, skb->len };
	len += skb->len;

	/* FIXME: linearize skb */
	{
		len = kernel_sendmsg(sock, &s->msg, iv, il, len);
	}
	kfree_skb(skb);

	if (len > 0) {
		s->dev->stats.tx_bytes += len;
		s->dev->stats.tx_packets++;
		return 0;
	}

	return len;
}
