static int async_chainiv_postpone_request(struct skcipher_givcrypt_request *req)
{
	struct crypto_ablkcipher *geniv = skcipher_givcrypt_reqtfm(req);
	struct async_chainiv_ctx *ctx = crypto_ablkcipher_ctx(geniv);
	int err;

	spin_lock_bh(&ctx->lock);
	err = skcipher_enqueue_givcrypt(&ctx->queue, req);
	spin_unlock_bh(&ctx->lock);

	if (test_and_set_bit(CHAINIV_STATE_INUSE, &ctx->state))
		return err;

	ctx->err = err;
	return async_chainiv_schedule_work(ctx);
}
