static inline int verify_sec_ctx_len(struct nlattr **attrs)
{
	struct nlattr *rt = attrs[XFRMA_SEC_CTX];
	struct xfrm_user_sec_ctx *uctx;

	if (!rt)
		return 0;

	uctx = nla_data(rt);
	if (uctx->len != (sizeof(struct xfrm_user_sec_ctx) + uctx->ctx_len))
		return -EINVAL;

	return 0;
}
