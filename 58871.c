unsigned char *skb_pull_rcsum(struct sk_buff *skb, unsigned int len)
{
	unsigned char *data = skb->data;

	BUG_ON(len > skb->len);
	__skb_pull(skb, len);
	skb_postpull_rcsum(skb, data, len);
	return skb->data;
}
