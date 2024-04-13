static int crypto_gcm_decrypt(struct aead_request *req)
{
	struct crypto_aead *aead = crypto_aead_reqtfm(req);
	struct crypto_gcm_req_priv_ctx *pctx = crypto_gcm_reqctx(req);
	struct ablkcipher_request *abreq = &pctx->u.abreq;
	struct crypto_gcm_ghash_ctx *gctx = &pctx->ghash_ctx;
	unsigned int authsize = crypto_aead_authsize(aead);
	unsigned int cryptlen = req->cryptlen;
	int err;

	if (cryptlen < authsize)
		return -EINVAL;
	cryptlen -= authsize;

	gctx->src = req->src;
	gctx->cryptlen = cryptlen;
	gctx->complete = gcm_dec_hash_done;

	err = gcm_hash(req, pctx);
	if (err)
		return err;

	ablkcipher_request_set_callback(abreq, aead_request_flags(req),
					gcm_decrypt_done, req);
	crypto_gcm_init_crypt(abreq, req, cryptlen);
	err = crypto_ablkcipher_decrypt(abreq);
	if (err)
		return err;

	return crypto_gcm_verify(req, pctx);
}
