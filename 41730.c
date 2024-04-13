static inline int copy_to_user_state_sec_ctx(struct xfrm_state *x, struct sk_buff *skb)
{
	if (x->security) {
		return copy_sec_ctx(x->security, skb);
	}
	return 0;
}
