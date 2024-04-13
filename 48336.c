static void async_chainiv_do_postponed(struct work_struct *work)
{
	struct async_chainiv_ctx *ctx = container_of(work,
						     struct async_chainiv_ctx,
						     postponed);
	struct skcipher_givcrypt_request *req;
	struct ablkcipher_request *subreq;
	int err;

	/* Only handle one request at a time to avoid hogging keventd. */
	spin_lock_bh(&ctx->lock);
	req = skcipher_dequeue_givcrypt(&ctx->queue);
	spin_unlock_bh(&ctx->lock);

	if (!req) {
		async_chainiv_schedule_work(ctx);
		return;
	}

	subreq = skcipher_givcrypt_reqctx(req);
	subreq->base.flags |= CRYPTO_TFM_REQ_MAY_SLEEP;

	err = async_chainiv_givencrypt_tail(req);

	local_bh_disable();
	skcipher_givcrypt_complete(req, err);
	local_bh_enable();
}
