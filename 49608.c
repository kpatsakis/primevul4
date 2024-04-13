static void khazad_decrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	struct khazad_ctx *ctx = crypto_tfm_ctx(tfm);
	khazad_crypt(ctx->D, dst, src);
}
