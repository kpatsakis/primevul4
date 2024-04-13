static void cryptd_blkcipher_crypt(struct ablkcipher_request *req,
				   struct crypto_blkcipher *child,
				   int err,
				   int (*crypt)(struct blkcipher_desc *desc,
						struct scatterlist *dst,
						struct scatterlist *src,
						unsigned int len))
{
	struct cryptd_blkcipher_request_ctx *rctx;
	struct blkcipher_desc desc;

	rctx = ablkcipher_request_ctx(req);

	if (unlikely(err == -EINPROGRESS))
		goto out;

	desc.tfm = child;
	desc.info = req->info;
	desc.flags = CRYPTO_TFM_REQ_MAY_SLEEP;

	err = crypt(&desc, req->dst, req->src, req->nbytes);

	req->base.complete = rctx->complete;

out:
	local_bh_disable();
	rctx->complete(&req->base, err);
	local_bh_enable();
}
