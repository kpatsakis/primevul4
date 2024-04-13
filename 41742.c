static inline size_t xfrm_acquire_msgsize(struct xfrm_state *x,
					  struct xfrm_policy *xp)
{
	return NLMSG_ALIGN(sizeof(struct xfrm_user_acquire))
	       + nla_total_size(sizeof(struct xfrm_user_tmpl) * xp->xfrm_nr)
	       + nla_total_size(sizeof(struct xfrm_mark))
	       + nla_total_size(xfrm_user_sec_ctx_size(x->security))
	       + userpolicy_type_attrsize();
}
