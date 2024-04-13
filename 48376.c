static int cryptd_blkcipher_enqueue(struct ablkcipher_request *req,
				    crypto_completion_t compl)
{
	struct cryptd_blkcipher_request_ctx *rctx = ablkcipher_request_ctx(req);
	struct crypto_ablkcipher *tfm = crypto_ablkcipher_reqtfm(req);
	struct cryptd_queue *queue;

	queue = cryptd_get_queue(crypto_ablkcipher_tfm(tfm));
	rctx->complete = req->base.complete;
	req->base.complete = compl;

	return cryptd_enqueue_request(queue, &req->base);
}
