static int chainiv_givencrypt(struct skcipher_givcrypt_request *req)
{
	struct crypto_ablkcipher *geniv = skcipher_givcrypt_reqtfm(req);
	struct chainiv_ctx *ctx = crypto_ablkcipher_ctx(geniv);
	struct ablkcipher_request *subreq = skcipher_givcrypt_reqctx(req);
	unsigned int ivsize;
	int err;

	ablkcipher_request_set_tfm(subreq, skcipher_geniv_cipher(geniv));
	ablkcipher_request_set_callback(subreq, req->creq.base.flags &
						~CRYPTO_TFM_REQ_MAY_SLEEP,
					req->creq.base.complete,
					req->creq.base.data);
	ablkcipher_request_set_crypt(subreq, req->creq.src, req->creq.dst,
				     req->creq.nbytes, req->creq.info);

	spin_lock_bh(&ctx->lock);

	ivsize = crypto_ablkcipher_ivsize(geniv);

	memcpy(req->giv, ctx->iv, ivsize);
	memcpy(subreq->info, ctx->iv, ivsize);

	err = crypto_ablkcipher_encrypt(subreq);
	if (err)
		goto unlock;

	memcpy(ctx->iv, subreq->info, ivsize);

unlock:
	spin_unlock_bh(&ctx->lock);

	return err;
}
