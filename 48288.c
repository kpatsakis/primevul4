static void crypto_authenc_esn_encrypt_done(struct crypto_async_request *req,
					    int err)
{
	struct aead_request *areq = req->data;

	if (!err) {
		struct crypto_aead *authenc_esn = crypto_aead_reqtfm(areq);
		struct crypto_authenc_esn_ctx *ctx = crypto_aead_ctx(authenc_esn);
		struct ablkcipher_request *abreq = aead_request_ctx(areq);
		u8 *iv = (u8 *)(abreq + 1) +
			 crypto_ablkcipher_reqsize(ctx->enc);

		err = crypto_authenc_esn_genicv(areq, iv, 0);
	}

	authenc_esn_request_complete(areq, err);
}
