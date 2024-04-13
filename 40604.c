static inline struct xfrm_user_sec_ctx *pfkey_sadb2xfrm_user_sec_ctx(const struct sadb_x_sec_ctx *sec_ctx)
{
	struct xfrm_user_sec_ctx *uctx = NULL;
	int ctx_size = sec_ctx->sadb_x_ctx_len;

	uctx = kmalloc((sizeof(*uctx)+ctx_size), GFP_KERNEL);

	if (!uctx)
		return NULL;

	uctx->len = pfkey_sec_ctx_len(sec_ctx);
	uctx->exttype = sec_ctx->sadb_x_sec_exttype;
	uctx->ctx_doi = sec_ctx->sadb_x_ctx_doi;
	uctx->ctx_alg = sec_ctx->sadb_x_ctx_alg;
	uctx->ctx_len = sec_ctx->sadb_x_ctx_len;
	memcpy(uctx + 1, sec_ctx + 1,
	       uctx->ctx_len);

	return uctx;
}
