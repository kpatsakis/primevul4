static int cryptd_hash_enqueue(struct ahash_request *req,
				crypto_completion_t compl)
{
	struct cryptd_hash_request_ctx *rctx = ahash_request_ctx(req);
	struct crypto_ahash *tfm = crypto_ahash_reqtfm(req);
	struct cryptd_queue *queue =
		cryptd_get_queue(crypto_ahash_tfm(tfm));

	rctx->complete = req->base.complete;
	req->base.complete = compl;

	return cryptd_enqueue_request(queue, &req->base);
}
