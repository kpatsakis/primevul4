static int crypto_gcm_encrypt(struct aead_request *req)
{
	struct crypto_gcm_req_priv_ctx *pctx = crypto_gcm_reqctx(req);
	struct ablkcipher_request *abreq = &pctx->u.abreq;
	struct crypto_gcm_ghash_ctx *gctx = &pctx->ghash_ctx;
	int err;

	crypto_gcm_init_crypt(abreq, req, req->cryptlen);
	ablkcipher_request_set_callback(abreq, aead_request_flags(req),
					gcm_encrypt_done, req);

	gctx->src = req->dst;
	gctx->cryptlen = req->cryptlen;
	gctx->complete = gcm_enc_hash_done;

	err = crypto_ablkcipher_encrypt(abreq);
	if (err)
		return err;

	err = gcm_hash(req, pctx);
	if (err)
		return err;

	crypto_xor(pctx->auth_tag, pctx->iauth_tag, 16);
	gcm_enc_copy_hash(req, pctx);

	return 0;
}
