static int chainiv_init(struct crypto_tfm *tfm)
{
	struct chainiv_ctx *ctx = crypto_tfm_ctx(tfm);

	spin_lock_init(&ctx->lock);

	return chainiv_init_common(tfm);
}
