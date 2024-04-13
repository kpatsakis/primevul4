__sum16 __skb_checksum_complete(struct sk_buff *skb)
{
	return __skb_checksum_complete_head(skb, skb->len);
}
