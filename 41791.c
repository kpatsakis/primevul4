static inline int xfrm_user_sec_ctx_size(struct xfrm_sec_ctx *xfrm_ctx)
{
	int len = 0;

	if (xfrm_ctx) {
		len += sizeof(struct xfrm_user_sec_ctx);
		len += xfrm_ctx->ctx_len;
	}
	return len;
}
