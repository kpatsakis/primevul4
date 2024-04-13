static void serpent_encrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	struct serpent_ctx *ctx = crypto_tfm_ctx(tfm);

	__serpent_encrypt(ctx, dst, src);
}
