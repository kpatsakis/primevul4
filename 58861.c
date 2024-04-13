struct sk_buff *skb_checksum_trimmed(struct sk_buff *skb,
				     unsigned int transport_len,
				     __sum16(*skb_chkf)(struct sk_buff *skb))
{
	struct sk_buff *skb_chk;
	unsigned int offset = skb_transport_offset(skb);
	__sum16 ret;

	skb_chk = skb_checksum_maybe_trim(skb, transport_len);
	if (!skb_chk)
		goto err;

	if (!pskb_may_pull(skb_chk, offset))
		goto err;

	skb_pull_rcsum(skb_chk, offset);
	ret = skb_chkf(skb_chk);
	skb_push_rcsum(skb_chk, offset);

	if (ret)
		goto err;

	return skb_chk;

err:
	if (skb_chk && skb_chk != skb)
		kfree_skb(skb_chk);

	return NULL;

}
