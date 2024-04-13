static inline void blowfish_enc_blk_xor(struct bf_ctx *ctx, u8 *dst,
					const u8 *src)
{
	__blowfish_enc_blk(ctx, dst, src, true);
}
