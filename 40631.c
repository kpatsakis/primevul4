static inline int pfkey_xfrm_policy2sec_ctx_size(const struct xfrm_policy *xp)
{
  struct xfrm_sec_ctx *xfrm_ctx = xp->security;

	if (xfrm_ctx) {
		int len = sizeof(struct sadb_x_sec_ctx);
		len += xfrm_ctx->ctx_len;
		return PFKEY_ALIGN8(len);
	}
	return 0;
}
