static struct sk_buff *receive_small(struct virtnet_info *vi, void *buf, unsigned int len)
{
	struct sk_buff * skb = buf;

	len -= vi->hdr_len;
	skb_trim(skb, len);

	return skb;
}
