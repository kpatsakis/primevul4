static void crypto_authenc_encrypt_done(struct crypto_async_request *req,
					int err)
{
	struct aead_request *areq = req->data;

	if (!err) {
		struct crypto_aead *authenc = crypto_aead_reqtfm(areq);
		struct crypto_authenc_ctx *ctx = crypto_aead_ctx(authenc);
		struct authenc_request_ctx *areq_ctx = aead_request_ctx(areq);
		struct ablkcipher_request *abreq = (void *)(areq_ctx->tail
							    + ctx->reqoff);
		u8 *iv = (u8 *)abreq - crypto_ablkcipher_ivsize(ctx->enc);

		err = crypto_authenc_genicv(areq, iv, 0);
	}

	authenc_request_complete(areq, err);
}
