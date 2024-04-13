static u8 *crypto_authenc_esn_ahash(struct aead_request *req,
				    unsigned int flags)
{
	struct crypto_aead *authenc_esn = crypto_aead_reqtfm(req);
	struct crypto_authenc_esn_ctx *ctx = crypto_aead_ctx(authenc_esn);
	struct crypto_ahash *auth = ctx->auth;
	struct authenc_esn_request_ctx *areq_ctx = aead_request_ctx(req);
	struct ahash_request *ahreq = (void *)(areq_ctx->tail + ctx->reqoff);
	u8 *hash = areq_ctx->tail;
	int err;

	hash = (u8 *)ALIGN((unsigned long)hash + crypto_ahash_alignmask(auth),
			    crypto_ahash_alignmask(auth) + 1);

	ahash_request_set_tfm(ahreq, auth);

	err = crypto_ahash_init(ahreq);
	if (err)
		return ERR_PTR(err);

	ahash_request_set_crypt(ahreq, areq_ctx->hsg, hash, areq_ctx->headlen);
	ahash_request_set_callback(ahreq, aead_request_flags(req) & flags,
				   areq_ctx->update_complete, req);

	err = crypto_ahash_update(ahreq);
	if (err)
		return ERR_PTR(err);

	ahash_request_set_crypt(ahreq, areq_ctx->sg, hash, areq_ctx->cryptlen);
	ahash_request_set_callback(ahreq, aead_request_flags(req) & flags,
				   areq_ctx->update_complete2, req);

	err = crypto_ahash_update(ahreq);
	if (err)
		return ERR_PTR(err);

	ahash_request_set_crypt(ahreq, areq_ctx->tsg, hash,
				areq_ctx->trailen);
	ahash_request_set_callback(ahreq, aead_request_flags(req) & flags,
				   areq_ctx->complete, req);

	err = crypto_ahash_finup(ahreq);
	if (err)
		return ERR_PTR(err);

	return hash;
}
