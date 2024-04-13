static void async_chainiv_exit(struct crypto_tfm *tfm)
{
	struct async_chainiv_ctx *ctx = crypto_tfm_ctx(tfm);

	BUG_ON(test_bit(CHAINIV_STATE_INUSE, &ctx->state) || ctx->queue.qlen);

	skcipher_geniv_exit(tfm);
}
