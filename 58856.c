static struct sk_buff *skb_checksum_maybe_trim(struct sk_buff *skb,
					       unsigned int transport_len)
{
	struct sk_buff *skb_chk;
	unsigned int len = skb_transport_offset(skb) + transport_len;
	int ret;

	if (skb->len < len)
		return NULL;
	else if (skb->len == len)
		return skb;

	skb_chk = skb_clone(skb, GFP_ATOMIC);
	if (!skb_chk)
		return NULL;

	ret = pskb_trim_rcsum(skb_chk, len);
	if (ret) {
		kfree_skb(skb_chk);
		return NULL;
	}

	return skb_chk;
}
