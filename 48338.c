static int async_chainiv_givencrypt(struct skcipher_givcrypt_request *req)
{
	struct crypto_ablkcipher *geniv = skcipher_givcrypt_reqtfm(req);
	struct async_chainiv_ctx *ctx = crypto_ablkcipher_ctx(geniv);
	struct ablkcipher_request *subreq = skcipher_givcrypt_reqctx(req);

	ablkcipher_request_set_tfm(subreq, skcipher_geniv_cipher(geniv));
	ablkcipher_request_set_callback(subreq, req->creq.base.flags,
					req->creq.base.complete,
					req->creq.base.data);
	ablkcipher_request_set_crypt(subreq, req->creq.src, req->creq.dst,
				     req->creq.nbytes, req->creq.info);

	if (test_and_set_bit(CHAINIV_STATE_INUSE, &ctx->state))
		goto postpone;

	if (ctx->queue.qlen) {
		clear_bit(CHAINIV_STATE_INUSE, &ctx->state);
		goto postpone;
	}

	return async_chainiv_givencrypt_tail(req);

postpone:
	return async_chainiv_postpone_request(req);
}
