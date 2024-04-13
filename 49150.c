static void camellia_encrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	struct camellia_sparc64_ctx *ctx = crypto_tfm_ctx(tfm);

	camellia_sparc64_crypt(&ctx->encrypt_key[0],
			       (const u32 *) src,
			       (u32 *) dst, ctx->key_len);
}
