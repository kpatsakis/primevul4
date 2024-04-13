static void gcm_dec_hash_done(struct aead_request *req, int err)
{
	struct crypto_gcm_req_priv_ctx *pctx = crypto_gcm_reqctx(req);
	struct ablkcipher_request *abreq = &pctx->u.abreq;
	struct crypto_gcm_ghash_ctx *gctx = &pctx->ghash_ctx;

	if (!err) {
		ablkcipher_request_set_callback(abreq, aead_request_flags(req),
						gcm_decrypt_done, req);
		crypto_gcm_init_crypt(abreq, req, gctx->cryptlen);
		err = crypto_ablkcipher_decrypt(abreq);
		if (err == -EINPROGRESS || err == -EBUSY)
			return;
		else if (!err)
			err = crypto_gcm_verify(req, pctx);
	}

	aead_request_complete(req, err);
}
