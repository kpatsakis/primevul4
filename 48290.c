static int crypto_authenc_esn_givencrypt(struct aead_givcrypt_request *req)
{
	struct crypto_aead *authenc_esn = aead_givcrypt_reqtfm(req);
	struct crypto_authenc_esn_ctx *ctx = crypto_aead_ctx(authenc_esn);
	struct aead_request *areq = &req->areq;
	struct skcipher_givcrypt_request *greq = aead_request_ctx(areq);
	u8 *iv = req->giv;
	int err;

	skcipher_givcrypt_set_tfm(greq, ctx->enc);
	skcipher_givcrypt_set_callback(greq, aead_request_flags(areq),
				       crypto_authenc_esn_givencrypt_done, areq);
	skcipher_givcrypt_set_crypt(greq, areq->src, areq->dst, areq->cryptlen,
				    areq->iv);
	skcipher_givcrypt_set_giv(greq, iv, req->seq);

	err = crypto_skcipher_givencrypt(greq);
	if (err)
		return err;

	return crypto_authenc_esn_genicv(areq, iv, CRYPTO_TFM_REQ_MAY_SLEEP);
}
