static void des_encrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	struct des_sparc64_ctx *ctx = crypto_tfm_ctx(tfm);
	const u64 *K = ctx->encrypt_expkey;

	des_sparc64_crypt(K, (const u64 *) src, (u64 *) dst);
}
