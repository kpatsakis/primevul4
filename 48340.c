static int async_chainiv_givencrypt_tail(struct skcipher_givcrypt_request *req)
{
	struct crypto_ablkcipher *geniv = skcipher_givcrypt_reqtfm(req);
	struct async_chainiv_ctx *ctx = crypto_ablkcipher_ctx(geniv);
	struct ablkcipher_request *subreq = skcipher_givcrypt_reqctx(req);
	unsigned int ivsize = crypto_ablkcipher_ivsize(geniv);

	memcpy(req->giv, ctx->iv, ivsize);
	memcpy(subreq->info, ctx->iv, ivsize);

	ctx->err = crypto_ablkcipher_encrypt(subreq);
	if (ctx->err)
		goto out;

	memcpy(ctx->iv, subreq->info, ivsize);

out:
	return async_chainiv_schedule_work(ctx);
}
