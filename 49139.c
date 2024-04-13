static void aes_encrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	struct crypto_sparc64_aes_ctx *ctx = crypto_tfm_ctx(tfm);

	ctx->ops->encrypt(&ctx->key[0], (const u32 *) src, (u32 *) dst);
}
