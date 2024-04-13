static inline struct sk_buff *pfkey_xfrm_state2msg_expire(const struct xfrm_state *x,
							  int hsc)
{
	return __pfkey_xfrm_state2msg(x, 0, hsc);
}
