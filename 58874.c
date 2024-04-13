static struct sk_buff *skb_reorder_vlan_header(struct sk_buff *skb)
{
	if (skb_cow(skb, skb_headroom(skb)) < 0) {
		kfree_skb(skb);
		return NULL;
	}

	memmove(skb->data - ETH_HLEN, skb->data - skb->mac_len - VLAN_HLEN,
		2 * ETH_ALEN);
	skb->mac_header += VLAN_HLEN;
	return skb;
}
