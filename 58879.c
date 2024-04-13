int skb_to_sgvec_nomark(struct sk_buff *skb, struct scatterlist *sg,
			int offset, int len)
{
	return __skb_to_sgvec(skb, sg, offset, len);
}
