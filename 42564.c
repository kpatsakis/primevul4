static unsigned int br_nf_local_in(unsigned int hook, struct sk_buff *skb,
				   const struct net_device *in,
				   const struct net_device *out,
				   int (*okfn)(struct sk_buff *))
{
	struct rtable *rt = skb_rtable(skb);

	if (rt && rt == bridge_parent_rtable(in))
		skb_dst_drop(skb);

	return NF_ACCEPT;
}
