struct sk_buff *pskb_extract(struct sk_buff *skb, int off,
			     int to_copy, gfp_t gfp)
{
	struct sk_buff  *clone = skb_clone(skb, gfp);

	if (!clone)
		return NULL;

	if (pskb_carve(clone, off, gfp) < 0 ||
	    pskb_trim(clone, to_copy)) {
		kfree_skb(clone);
		return NULL;
	}
	return clone;
}
