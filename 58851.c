static int pskb_carve(struct sk_buff *skb, const u32 len, gfp_t gfp)
{
	int headlen = skb_headlen(skb);

	if (len < headlen)
		return pskb_carve_inside_header(skb, len, headlen, gfp);
	else
		return pskb_carve_inside_nonlinear(skb, len, headlen, gfp);
}
