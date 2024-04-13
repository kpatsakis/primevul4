static inline int copy_to_user_sec_ctx(struct xfrm_policy *xp, struct sk_buff *skb)
{
	if (xp->security)
		return copy_sec_ctx(xp->security, skb);
	return 0;
}
