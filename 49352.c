static inline void des3_ede_dec_blk_3way(struct des3_ede_x86_ctx *ctx, u8 *dst,
					 const u8 *src)
{
	u32 *dec_ctx = ctx->dec_expkey;

	des3_ede_x86_64_crypt_blk_3way(dec_ctx, dst, src);
}
