static void des3_ede_encrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	struct des3_ede_sparc64_ctx *ctx = crypto_tfm_ctx(tfm);
	const u64 *K = ctx->encrypt_expkey;

	des3_ede_sparc64_crypt(K, (const u64 *) src, (u64 *) dst);
}
