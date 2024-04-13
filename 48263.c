static u8 *crypto_authenc_ahash(struct aead_request *req, unsigned int flags)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
	struct crypto_ahash *auth = ctx->auth;
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ctx->reqoff);
	u8 *hash = areq_ctx->tail;
	int err;

	hash = (u8 *)ALIGN((unsigned long)hash + crypto_ahash_alignmask(auth),
			   crypto_ahash_alignmask(auth) + 1);

	ahash_request_set_tfm(ahreq, auth);
	ahash_request_set_crypt(ahreq, areq_ctx->sg, hash,
				areq_ctx->cryptlen);
	ahash_request_set_callback(ahreq, aead_request_flags(req) & flags,
				   areq_ctx->complete, req);

	err = crypto_ahash_digest(ahreq);
	if (err)
		return ERR_PTR(err);

	return hash;
}
