static int async_chainiv_init(struct crypto_tfm *tfm)
{
	struct async_chainiv_ctx *ctx = crypto_tfm_ctx(tfm);

	spin_lock_init(&ctx->lock);

	crypto_init_queue(&ctx->queue, 100);
	INIT_WORK(&ctx->postponed, async_chainiv_do_postponed);

	return chainiv_init_common(tfm);
}
