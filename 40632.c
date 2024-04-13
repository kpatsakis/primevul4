static inline struct sk_buff *pfkey_xfrm_state2msg(const struct xfrm_state *x)
{
	struct sk_buff *skb;

	skb = __pfkey_xfrm_state2msg(x, 1, 3);

	return skb;
}
