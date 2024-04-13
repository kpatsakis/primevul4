static inline void des3_ede_enc_blk_3way(struct des3_ede_x86_ctx *ctx, u8 *dst,
					 const u8 *src)
{
	u32 *enc_ctx = ctx->enc_expkey;

	des3_ede_x86_64_crypt_blk_3way(enc_ctx, dst, src);
}
