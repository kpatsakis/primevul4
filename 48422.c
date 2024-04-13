static int crypto_rfc3686_crypt(struct ablkcipher_request *req)
{
	struct crypto_ablkcipher *tfm = crypto_ablkcipher_reqtfm(req);
	struct crypto_rfc3686_ctx *ctx = crypto_ablkcipher_ctx(tfm);
	struct crypto_ablkcipher *child = ctx->child;
	unsigned long align = crypto_ablkcipher_alignmask(tfm);
	struct crypto_rfc3686_req_ctx *rctx =
		(void *)PTR_ALIGN((u8 *)ablkcipher_request_ctx(req), align + 1);
	struct ablkcipher_request *subreq = &rctx->subreq;
	u8 *iv = rctx->iv;

	/* set up counter block */
	memcpy(iv, ctx->nonce, CTR_RFC3686_NONCE_SIZE);
	memcpy(iv + CTR_RFC3686_NONCE_SIZE, req->info, CTR_RFC3686_IV_SIZE);

	/* initialize counter portion of counter block */
	*(__be32 *)(iv + CTR_RFC3686_NONCE_SIZE + CTR_RFC3686_IV_SIZE) =
		cpu_to_be32(1);

	ablkcipher_request_set_tfm(subreq, child);
	ablkcipher_request_set_callback(subreq, req->base.flags,
					req->base.complete, req->base.data);
	ablkcipher_request_set_crypt(subreq, req->src, req->dst, req->nbytes,
				     iv);

	return crypto_ablkcipher_encrypt(subreq);
}
