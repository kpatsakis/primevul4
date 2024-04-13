void crypto_aes_encrypt_x86(struct crypto_aes_ctx *ctx, u8 *dst, const u8 *src)
{
	aes_enc_blk(ctx, dst, src);
}
