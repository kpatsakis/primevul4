static inline int skb_needs_linearize(struct sk_buff *skb,
				      int features)
{
	return skb_is_nonlinear(skb) &&
			((skb_has_frag_list(skb) &&
				!(features & NETIF_F_FRAGLIST)) ||
			(skb_shinfo(skb)->nr_frags &&
				!(features & NETIF_F_SG)));
}
