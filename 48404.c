static void cryptd_hash_update(struct crypto_async_request *req_async, int err)
{
	struct ahash_request *req = ahash_request_cast(req_async);
	struct cryptd_hash_request_ctx *rctx;

	rctx = ahash_request_ctx(req);

	if (unlikely(err == -EINPROGRESS))
		goto out;

	err = shash_ahash_update(req, &rctx->desc);

	req->base.complete = rctx->complete;

out:
	local_bh_disable();
	rctx->complete(&req->base, err);
	local_bh_enable();
}
