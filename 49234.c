static inline void blowfish_enc_blk_4way(struct bf_ctx *ctx, u8 *dst,
					 const u8 *src)
{
	__blowfish_enc_blk_4way(ctx, dst, src, false);
}
