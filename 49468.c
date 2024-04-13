static void nx842_exit(struct crypto_tfm *tfm)
{
	struct nx842_ctx *ctx = crypto_tfm_ctx(tfm);

	kfree(ctx->nx842_wmem);
}
