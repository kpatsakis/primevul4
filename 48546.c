static int mcryptd_hash_enqueue(struct ahash_request *req,
				crypto_completion_t complete)
{
	int ret;

	struct mcryptd_hash_request_ctx *rctx = ahash_request_ctx(req);
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct mcryptd_queue *queue =
		mcryptd_get_queue(crypto_ahash_tfm(tfm));

	rctx->complete = req->base.complete;
	req->base.complete = complete;

	ret = mcryptd_enqueue_request(queue, &req->base, rctx);

	return ret;
}
