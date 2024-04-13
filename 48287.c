static int crypto_authenc_esn_encrypt(struct aead_request *req)
{
	struct crypto_aead *authenc_esn = crypto_aead_reqtfm(req);
	struct crypto_authenc_esn_ctx *ctx = crypto_aead_ctx(authenc_esn);
	struct authenc_esn_request_ctx *areq_ctx = aead_request_ctx(req);
	struct crypto_ablkcipher *enc = ctx->enc;
	struct scatterlist *dst = req->dst;
	unsigned int cryptlen = req->cryptlen;
	struct ablkcipher_request *abreq = (void *)(areq_ctx->tail
						    + ctx->reqoff);
	u8 *iv = (u8 *)abreq - crypto_ablkcipher_ivsize(enc);
	int err;

	ablkcipher_request_set_tfm(abreq, enc);
	ablkcipher_request_set_callback(abreq, aead_request_flags(req),
					crypto_authenc_esn_encrypt_done, req);
	ablkcipher_request_set_crypt(abreq, req->src, dst, cryptlen, req->iv);

	memcpy(iv, req->iv, crypto_aead_ivsize(authenc_esn));

	err = crypto_ablkcipher_encrypt(abreq);
	if (err)
		return err;

	return crypto_authenc_esn_genicv(req, iv, CRYPTO_TFM_REQ_MAY_SLEEP);
}
