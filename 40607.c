static inline int pfkey_sec_ctx_len(const struct sadb_x_sec_ctx *sec_ctx)
{
	return DIV_ROUND_UP(sizeof(struct sadb_x_sec_ctx) +
			    sec_ctx->sadb_x_ctx_len,
			    sizeof(uint64_t));
}
