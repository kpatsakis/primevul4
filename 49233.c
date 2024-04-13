static inline void blowfish_enc_blk(struct bf_ctx *ctx, u8 *dst, const u8 *src)
{
	__blowfish_enc_blk(ctx, dst, src, false);
}
