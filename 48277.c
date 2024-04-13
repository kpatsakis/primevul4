static int crypto_authenc_verify(struct aead_request *req,
				 authenc_ahash_t authenc_ahash_fn)
{
	struct crypto_aead *authenc = crypto_aead_reqtfm(req);
	struct authenc_request_ctx *areq_ctx = aead_request_ctx(req);
	u8 *ohash;
	u8 *ihash;
	unsigned int authsize;

	areq_ctx->complete = authenc_verify_ahash_done;
	areq_ctx->update_complete = authenc_verify_ahash_update_done;

	ohash = authenc_ahash_fn(req, CRYPTO_TFM_REQ_MAY_SLEEP);
	if (IS_ERR(ohash))
		return PTR_ERR(ohash);

	authsize = crypto_aead_authsize(authenc);
	ihash = ohash + authsize;
	scatterwalk_map_and_copy(ihash, areq_ctx->sg, areq_ctx->cryptlen,
				 authsize, 0);
	return crypto_memneq(ihash, ohash, authsize) ? -EBADMSG : 0;
}
