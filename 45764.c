static unsigned int unix_skb_len(const struct sk_buff *skb)
{
	return skb->len - UNIXCB(skb).consumed;
}
