static struct aead_request *crypto_rfc4106_crypt(struct aead_request *req)
{
	struct aead_request *subreq = aead_request_ctx(req);
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct crypto_rfc4106_ctx *ctx = crypto_aead_ctx(aead);
	struct crypto_aead *child = ctx->child;
	u8 *iv = PTR_ALIGN((u8 *)(subreq + 1) + crypto_aead_reqsize(child),
			   crypto_aead_alignmask(child) + 1);

	memcpy(iv, ctx->nonce, 4);
	memcpy(iv + 4, req->iv, 8);

	aead_request_set_tfm(subreq, child);
	aead_request_set_callback(subreq, req->base.flags, req->base.complete,
				  req->base.data);
	aead_request_set_crypt(subreq, req->src, req->dst, req->cryptlen, iv);
	aead_request_set_assoc(subreq, req->assoc, req->assoclen);

	return subreq;
}
