static void anubis_encrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	struct anubis_ctx *ctx = crypto_tfm_ctx(tfm);
	anubis_crypt(ctx->E, dst, src, ctx->R);
}
