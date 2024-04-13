static void cryptd_aead_crypt(struct aead_request *req,
			struct crypto_aead *child,
			int err,
			int (*crypt)(struct aead_request *req))
{
	struct cryptd_aead_request_ctx *rctx;
	rctx = aead_request_ctx(req);

	if (unlikely(err == -EINPROGRESS))
		goto out;
	aead_request_set_tfm(req, child);
	err = crypt( req );
	req->base.complete = rctx->complete;
out:
	local_bh_disable();
	rctx->complete(&req->base, err);
	local_bh_enable();
}
